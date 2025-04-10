#ifndef _MODULE_HPP
#define _MODULE_HPP

#include "object.hpp"
#include "module_item.hpp"

class Module : public CanvasObject {
    public:
        Module(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject (canvas, attributes) {};
    
        std::string name();
        long id();
        std::vector<ModuleItem> items();
    
    private:
        CanvasObject* course;
    };

#endif//_MODULE_HPP