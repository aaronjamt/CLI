#ifndef _ASSIGNMENT_HPP
#define _ASSIGNMENT_HPP

#include "object.hpp"
#include "submission.hpp"

#include <optional>

class Assignment : public CanvasObject {
    public:
        Assignment(CanvasAPI* canvas, CanvasObject *course, nlohmann::json attributes) : CanvasObject(canvas, attributes), course(course) {}

        std::optional<Submission> submission();

        // How many points is this assignment worth?
        std::optional<double> value();

    private:
        CanvasObject *course;
};

#endif//_ASSIGNMENT_HPP