#ifndef _QUIZ_HPP
#define _QUIZ_HPP

#include "object.hpp"

class Quiz : public CanvasObject {
    public:
        Quiz(CanvasAPI* canvas, CanvasObject *course, nlohmann::json attributes) : CanvasObject(canvas, attributes), course(course) {}

        char *name();
        bool validate_access_code(char* code);

    private:
        CanvasObject *course;
};

#endif//_QUIZ_HPP