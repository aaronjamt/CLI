#include "submission.hpp"
#include "assignment.hpp"

std::optional<double> Submission::score() {
    // Make sure we have a score
    if (!attributes["score"].is_number()) return std::nullopt;

    double points = attributes.value("score", -1.0);

    // Make sure we have a valid "out_of" value
    std::optional<double> out_of = ((Assignment*)assignment)->value();
    if (!out_of.has_value() || out_of.value() == 0) return std::nullopt;
    
    return points / out_of.value() * 100;
}