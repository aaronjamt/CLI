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