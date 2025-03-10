#ifndef _COURSE_HPP
#define _COURSE_HPP

#include "assignment.hpp"
#include "object.hpp"
#include "quiz.hpp"

class Course : public CanvasObject {
    public:
        using CanvasObject::CanvasObject;
        Course(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {
		    // get_final_score();

	    }

        std::vector<Assignment> assignments();
        std::vector<Quiz> quizzes();

        double get_current_grade();
        double get_final_score();
};

#endif//_COURSE_HPP
