#ifndef _MODULE_ITEM_HPP
#define _MODULE_ITEM_HPP

#include "object.hpp"

class ModuleItem : public CanvasObject {
    public:
        ModuleItem(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}
    
        std::string title();
        std::string type();
    };

#endif//_MODULE_ITEM_HPP