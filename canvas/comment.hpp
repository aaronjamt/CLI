#ifndef _COMMENT_HPP
#define _COMMENT_HPP

#include "object.hpp"

class Comment : public CanvasObject {
    public:
        Comment(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
};

#endif//_COMMENT_HPP