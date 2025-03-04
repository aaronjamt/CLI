#ifndef _COURSE_HPP
#define _COURSE_HPP

#include "object.hpp"

class Course : public CanvasObject {
    public:
        using CanvasObject::CanvasObject;
        Course(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
};

#endif//_COURSE_HPP