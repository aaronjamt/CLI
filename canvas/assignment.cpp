#include "assignment.hpp"
#include <optional>

std::optional<Submission> Assignment::submission() {
    if (this->attributes.is_object())
        return Submission(api, this, this->attributes["submission"]);
    return std::nullopt;
}

std::optional<double> Assignment::value() {
    if (attributes["points_possible"].is_number()) {
        return attributes["points_possible"].get<double>();
    }
    return std::nullopt;
}

char* Assignment::posted_at() {
    if (attributes["created_at"].is_string()) {
        std::string when = attributes["created_at"].get<std::string>();
        // TODO: ISO 8601 conversion
        return strdup(when.c_str());
    }
    return (char*)"(unknown posted_at)";
}

char* Assignment::due_date() {
    if (attributes["due_at"].is_string()) {
        std::string when = attributes["due_at"].get<std::string>();
        // TODO: ISO 8601 conversion
        return strdup(when.c_str());
    }
    return (char*)"(unknown due_date)";
}
