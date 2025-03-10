#ifndef _SUBMISSION_HPP
#define _SUBMISSION_HPP

#include "object.hpp"

#include <optional>

class Submission : public CanvasObject {
    public:
        Submission(CanvasAPI* canvas, CanvasObject *assignment, nlohmann::json attributes) : CanvasObject(canvas, attributes), assignment(assignment) {}

        std::optional<double> score();

    private:
        CanvasObject *assignment;
};

#endif//_SUBMISSION_HPP