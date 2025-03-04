#ifndef _ASSIGNMENT_HPP
#define _ASSIGNMENT_HPP

#include "object.hpp"

class Assignment : public CanvasObject {
    public:
        Assignment(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
};

#endif//_ASSIGNMENT_HPP