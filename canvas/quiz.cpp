#include "quiz.hpp"

// Quizzes use the "title" attribute for their name
char* Quiz::name() {
    if (!attributes["title"].is_string()) return NULL;
    return strdup(attributes["title"].get<std::string>().c_str());
}

bool Quiz::validate_access_code(char* code) {
    std::string endpoint = "/api/v1/courses/" +
        std::to_string(course->id()) +
        "/quizzes/" +
        std::to_string(id()) +
        "/validate_access_code";
    nlohmann::json data;
    data["access_code"] = code;
    nlohmann::json response = api->_request(endpoint.c_str(), data);

    // The response should be a boolean
    if (response.is_boolean()) {
        return response.get<bool>();
    }
    return false;
}