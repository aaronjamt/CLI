#ifndef _ASSIGNMENT_HPP
#define _ASSIGNMENT_HPP

#include "object.hpp"
#include "file.hpp"
#include "submission.hpp"

#include <optional>

class Assignment : public CanvasObject {
    public:
        Assignment(CanvasAPI* canvas, CanvasObject *course, nlohmann::json attributes) : CanvasObject(canvas, attributes), course(course) {}

        std::optional<Submission> submission();

        std::optional<File> upload_file(const char* path);
        bool submit(Submission submission);

        // How many points is this assignment worth?
        std::optional<double> value();

        char* posted_at();
        char* due_date();

    private:
        CanvasObject *course;
};

#endif//_ASSIGNMENT_HPP