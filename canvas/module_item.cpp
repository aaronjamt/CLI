#include "module_item.hpp"

std::string ModuleItem::title() {
    return attributes["title"];
}

std::string ModuleItem::type() {
    return attributes["type"];
}