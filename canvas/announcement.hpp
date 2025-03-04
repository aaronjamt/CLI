#ifndef _ANNOUNCEMENT_HPP
#define _ANNOUNCEMENT_HPP

#include "object.hpp"

class Announcement : public CanvasObject {
    public:
        Announcement(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
};

#endif//_ANNOUNCEMENT_HPP