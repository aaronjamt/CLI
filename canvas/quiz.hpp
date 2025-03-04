#ifndef _QUIZ_HPP
#define _QUIZ_HPP

#include "object.hpp"

class Quiz : public CanvasObject {
    public:
        Quiz(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
};

#endif//_QUIZ_HPP