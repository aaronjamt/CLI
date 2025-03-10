#include "course.hpp"

std::vector<Assignment> Course::assignments() {
    std::vector<Assignment> assignments;
    std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/assignments";
    for (nlohmann::json assignment : api->_request(endpoint.c_str())) {
        assignments.push_back(Assignment(api, assignment));
    }
    return assignments;
}

std::vector<Quiz> Course::quizzes() {
    std::vector<Quiz> quizzes;
    std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/quizzes";
    for (nlohmann::json quiz : api->_request(endpoint.c_str())) {
        quizzes.push_back(Quiz(api, this, quiz));
    }
    return quizzes;
}

double Course::get_current_grade() {
    if (!attributes.contains("enrollments")) {
        return -1;
    }

    if (!attributes["enrollments"].is_array()) {
        return -1;
    }

    if (attributes["enrollments"].size() < 1) {
        return -1;
    }

    if (!attributes["enrollments"][0].is_object()) {
        return -1;
    }

    if (!attributes["enrollments"][0]["computed_current_score"].is_number()) {
        return -1;
    }

    return attributes["enrollments"][0]["computed_current_score"].get<double>();
}

double Course::get_final_score() {
    return attributes["enrollments"][0]["computed_final_score"].get<double>();
}