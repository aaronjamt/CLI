#ifndef _DISCUSSION_HPP
#define _DISCUSSION_HPP

#include "object.hpp"

class Discussion : public CanvasObject {
    public:
        Discussion(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}

        char* name();
        char* body();
        char* poster_name();

        char* posted_at();
};

#endif//_DISCUSSION_HPP