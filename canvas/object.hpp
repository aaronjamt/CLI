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

        long id() {
            if (!attributes["id"].is_number_unsigned()) return -1;
            return attributes["id"];
        }

        char *name() {
            if (!attributes["name"].is_string()) return NULL;
            return strdup(attributes["name"].get<std::string>().c_str());
        }

    protected:
        CanvasAPI *api;
        nlohmann::json attributes;
};

#endif//_OBJECT_HPP