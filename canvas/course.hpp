#ifndef _COURSE_HPP
#define _COURSE_HPP

#include "assignment.hpp"
#include "discussion.hpp"
#include "object.hpp"
#include "quiz.hpp"
#include "module.hpp"

class Course : public CanvasObject {
    public:
        using CanvasObject::CanvasObject;
        Course(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}

        std::vector<Discussion> announcements();
        std::vector<Discussion> discussions();
        std::vector<Assignment> assignments();
        std::vector<Quiz> quizzes();
        std::vector<Module> modules();

        std::optional<Assignment> assignment(std::string id);

        std::optional<double> get_current_grade();
        std::optional<double> get_final_score();
};

#endif//_COURSE_HPP
