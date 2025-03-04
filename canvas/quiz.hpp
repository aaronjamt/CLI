#ifndef _QUIZ_HPP
#define _QUIZ_HPP

#include "object.hpp"

class Quiz : public CanvasObject {
    public:
        Quiz(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}

        // Quizzes use the "title" attribute for their name
        char *name() {
            if (!attributes["title"].is_string()) return NULL;
            return strdup(attributes["title"].get<std::string>().c_str());
        }
};

#endif//_QUIZ_HPP