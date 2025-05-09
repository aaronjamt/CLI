#include "course.hpp"
#include "discussion.hpp"

std::vector<Discussion> Course::announcements() {
    std::vector<Discussion> announcements;
    std::string endpoint = "/api/v1/announcements?context_codes[]=course_" + std::to_string(id());
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    if (!response) {
        return {};
    }

    for (nlohmann::json announcement : response.value()) {
        announcements.push_back(Discussion(api, announcement));
    }
    return announcements;
}

std::vector<Assignment> Course::assignments() {
    std::vector<Assignment> assignments;
    std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/assignments?include[]=submission";
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    if (!response) {
        return {};
    }

    for (nlohmann::json assignment : response.value()) {
        assignments.push_back(Assignment(api, this, assignment));
    }
    return assignments;
}

std::vector<Module> Course::modules() {
    std::vector<Module> modules;
    std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/modules";
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    if (!response) {
        return {};
    }

    for (nlohmann::json module : response.value()) {
        modules.push_back(Module(api, module));
    }
    return modules;
}

std::vector<Discussion> Course::discussions() {
    std::vector<Discussion> discussions;
    std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/discussion_topics";
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    if (!response) {
        return {};
    }

    for (nlohmann::json assignment : response.value()) {
        discussions.push_back(Discussion(api, assignment));
    }
    return discussions;
}

std::vector<Quiz> Course::quizzes() {
    std::vector<Quiz> quizzes;
    std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/quizzes";
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    if (!response) {
        return {};
    }

    for (nlohmann::json quiz : response.value()) {
        quizzes.push_back(Quiz(api, this, quiz));
    }
    return quizzes;
}

std::optional<Assignment> Course::assignment(std::string id) {
    std::string endpoint = "/api/v1/courses/" + std::to_string(this->id()) + "/assignments/" + id;
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str());
    if (!response) {
        return {};
    }

    return Assignment(api, this, response.value());
}


std::optional<double> Course::get_current_grade() {
    if (!attributes.contains("enrollments")) {
        return std::nullopt;
    }

    if (!attributes["enrollments"].is_array()) {
        return std::nullopt;
    }

    if (attributes["enrollments"].size() < 1) {
        return std::nullopt;
    }

    if (!attributes["enrollments"][0].is_object()) {
        return std::nullopt;
    }

    if (!attributes["enrollments"][0]["computed_current_score"].is_number()) {
        return std::nullopt;
    }

    return attributes["enrollments"][0]["computed_current_score"].get<double>();
}

std::optional<double> Course::get_final_score() {
    if (!attributes.contains("enrollments")) {
        return std::nullopt;
    }

    if (!attributes["enrollments"].is_array()) {
        return std::nullopt;
    }

    if (attributes["enrollments"].size() < 1) {
        return std::nullopt;
    }

    if (!attributes["enrollments"][0].is_object()) {
        return std::nullopt;
    }

    if (!attributes["enrollments"][0]["computed_final_score"].is_number()) {
        return std::nullopt;
    }

    return attributes["enrollments"][0]["computed_final_score"].get<double>();
}