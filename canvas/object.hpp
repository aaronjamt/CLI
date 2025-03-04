#ifndef _OBJECT_HPP
#define _OBJECT_HPP

#include "api.hpp"
#include "nlohmann/json.hpp"

class CanvasObject {
    public:
        // Constructor stores the provided Canvas API and attributes
        CanvasObject(CanvasAPI *api, nlohmann::json attributes) : api(api), attributes(attributes) {}

        // Overload the [] operator to access attributes
        auto& operator[](char* key) {
            return attributes[key];
        }

        uint32_t id() {
            return attributes["id"];
        }

        char *name() {
            return strdup(attributes["name"].get<std::string>().c_str());
        }

    protected:
        CanvasAPI *api;
        nlohmann::json attributes;
};

#endif//_OBJECT_HPP