#include "quiz.hpp"
#include "iso8601.hpp"
#include "nlohmann/json.hpp"
#include <unistd.h>

// Quizzes use the "title" attribute for their name
char* Quiz::name() {
    if (!attributes["title"].is_string()) return NULL;
    return strdup(attributes["title"].get<std::string>().c_str());
}

char* Quiz::posted_at() {
    if (attributes["created_at"].is_string()) {
        std::string when = attributes["created_at"].get<std::string>();
        when = ISO8601::format_to_human(when);
        return strdup(when.c_str());
    }
    return (char*)"";
}

char* Quiz::due_date() {
    if (attributes["due_at"].is_string()) {
        std::string when = attributes["due_at"].get<std::string>();
        when = ISO8601::format_to_human(when);
        return strdup(when.c_str());
    }
    return (char*)"";
}

std::optional<QuizSubmission> Quiz::start_quiz(char *access_code) {
    std::string endpoint = "/api/v1/courses/" +
        std::to_string(course->id()) +
        "/quizzes/" +
        std::to_string(id()) +
        "/submissions";
    nlohmann::json data;
    if (access_code != NULL)
        data["access_code"] = access_code;
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str(), data);

    if (response) {
        nlohmann::json response_json = response.value();
        if (!response_json.contains("quiz_submissions")) {
            return {};
        }
        response_json = response_json["quiz_submissions"];
        if (!response_json.is_array() || response_json.size() != 1) {
            return {};
        }

        return QuizSubmission(api, course, id(), access_code, response_json[0]);
    }
    return {};
}

std::vector<QuizQuestion> Quiz::questions(std::optional<QuizSubmission> submission) {
    std::string endpoint = "/api/v1/courses/" +
        std::to_string(course->id()) +
        "/quizzes/" +
        std::to_string(id()) +
        "/questions";
    if (submission) {
        endpoint += "?quiz_submission_id=" + std::to_string(submission->id());
        if (!submission->attempt()) return {};
        endpoint += "&quiz_submission_attempt=" + std::to_string(submission->attempt().value());
    }
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    std::vector<QuizQuestion> questions;
    if (response) {
        for (nlohmann::json question : response.value()) {
            questions.push_back(QuizQuestion(api, question));
        }
    }
    return questions;
}

bool Quiz::validate_access_code(char* code) {
    std::string endpoint = "/api/v1/courses/" +
        std::to_string(course->id()) +
        "/quizzes/" +
        std::to_string(id()) +
        "/validate_access_code";
    nlohmann::json data;
    data["access_code"] = code;
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str(), data);

    // The response should be a boolean
    if (response && response->is_boolean()) {
        return response->get<bool>();
    }
    return false;
}

bool Quiz::has_access_code() {
    return attributes.contains("has_access_code") && attributes["has_access_code"].get<bool>();
}

// ============================================================== //
//                      QuizSubmission                            //
// ============================================================== //

std::optional<int> QuizSubmission::attempt() {
    if (attributes["attempt"].is_number()) {
        return attributes["attempt"];
    }
    return {};
}

std::vector<QuizQuestion>& QuizSubmission::questions() {
    std::string endpoint = "/api/v1/quiz_submissions/" +
        std::to_string(id()) +
        "/questions";

    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    
    if (response && response->contains("quiz_submission_questions")) {
        _questions.clear();
        for (nlohmann::json question : response.value()["quiz_submission_questions"]) {
            _questions.push_back(QuizQuestion(api, question));
        }
    }
    return _questions;
}

bool QuizSubmission::update_answers() {
    std::string endpoint = "/api/v1/quiz_submissions/" +
        std::to_string(id()) +
        "/questions";

    nlohmann::json json_answers;
    for (QuizQuestion question : _questions) {
        printf("Checking answer for question %s\n", question.name());
        if (question.answer().has_value()) {
            json_answers.push_back({
                {"id", question.id()},
                {"answer", question.answer().value()}
            });
        }
    }

    nlohmann::json data = {
        {"attempt", attempt().value()},
        {"validation_token", attributes["validation_token"]},
        {"quiz_questions", json_answers}
    };

    if (access_code != NULL) {
        data["access_code"] = access_code;
    }

    printf("POST data for update_answers: %s\n", data.dump().c_str());

    // Make the response and return whether it succeeded (we don't care about the response,
    // only whether there was one).
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str(), data);
    return response.has_value();
}

bool QuizSubmission::submit() {
    std::string endpoint = "/api/v1/courses/" +
        std::to_string(course->id()) +
        "/quizzes/" +
        std::to_string(quiz_id) +
        "/submissions/" +
        std::to_string(id()) +
        "/complete";

    if (!attempt().has_value() || !attributes.contains("validation_token")) {
        return false;
    }

    nlohmann::json data = {
        {"attempt", attempt().value()},
        {"validation_token", attributes["validation_token"]}
    };

    if (access_code != NULL) {
        data["access_code"] = access_code;
    }

    // Make the response and return whether it succeeded (we don't care about the response,
    // only whether there was one).
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str(), data);
    return response.has_value();
}

// ============================================================== //
//                       QuizQuestion                             //
// ============================================================== //

// Quizzes use the "question_text" attribute for their name
char* QuizQuestion::name() {
    if (!attributes["question_text"].is_string()) return NULL;
    return strdup(attributes["question_text"].get<std::string>().c_str());
}

nlohmann::json QuizQuestion::get_answers_json() {
    nlohmann::json json = {};
    if (attributes.contains("answers")) {
        for (nlohmann::json answer : attributes["answers"]) {
            json.push_back({
                {"id", answer["id"]},
                {"answer", answer["text"]}
            });
        }
    }
    return json;
}

std::vector<std::string> QuizQuestion::get_answers() {
    std::vector<std::string> answers;
    if (attributes.contains("answers")) {
        for (nlohmann::json answer : attributes["answers"]) {
            if (answer.contains("text") && answer["text"].get<std::string>().length() > 0) {
                answers.push_back(answer["text"]);
            } else if (answer.contains("html")) {
                // TODO: Parse HTML
                answers.push_back(answer["html"]);
            } else {
                // TODO: Handle other types of answers
                answers.push_back(answer.dump());
            }
        }
    }
    return answers;
}

bool QuizQuestion::set_answer(int answer) {
    if (type() == MultipleChoice || type() == TrueFalse) {
        if (attributes.contains("answers")) {
            attributes["answer"] = attributes["answers"][answer]["id"];
            return true;
        }
    } else if (type() == Number) {
        attributes["answer"] = answer;
        return true;
    }
    return false;
}

bool QuizQuestion::set_answer(char *answer) {
    attributes["answer"] = answer;
    return true;
}

std::optional<long> QuizQuestion::answer() {
    if (attributes.contains("answer") && attributes["answer"].is_number()) {
        return attributes["answer"];
    }
    return {};
}

QuizQuestionType QuizQuestion::type() {
    if (attributes.contains("question_type")) {
        std::string type = attributes["question_type"];
        if (type == "multiple_choice_question") {
            return MultipleChoice;
        } else if (type == "true_false_question") {
            return TrueFalse;
        } else if (type == "numerical_question") {
            return Number;
        } else if (type == "short_answer_question") {
            return Text;
        }
    }
    return Empty;
}