#ifndef _USER_HPP
#define _USER_HPP

#include "object.hpp"

class User : public CanvasObject {
    public:
        User(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
};

#endif//_USER_HPP