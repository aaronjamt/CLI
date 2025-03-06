#ifndef _COURSE_HPP
#define _COURSE_HPP

#include "assignment.hpp"
#include "object.hpp"
#include "quiz.hpp"
#include <iostream>

class Course : public CanvasObject {
    public:
        using CanvasObject::CanvasObject;
        Course(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {
		get_final_score();
	}

        std::vector<Assignment> assignments() {
            std::vector<Assignment> assignments;
            std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/assignments";
            for (nlohmann::json assignment : api->_request(endpoint.c_str())) {
                assignments.push_back(Assignment(api, assignment));
            }
            return assignments;
        }

        std::vector<Quiz> quizzes() {
            std::vector<Quiz> quizzes;
            std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/quizzes";
            for (nlohmann::json quiz : api->_request(endpoint.c_str())) {
                quizzes.push_back(Quiz(api, quiz));
            }
            return quizzes;
        }

        double get_final_score();
};

#endif//_COURSE_HPP
