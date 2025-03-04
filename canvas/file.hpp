#ifndef _FILE_HPP
#define _FILE_HPP

#include "object.hpp"

class File : public CanvasObject {
    public:
        File(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
};

#endif//_FILE_HPP