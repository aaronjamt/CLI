#include "discussion.hpp"

// Discussions use the "title" attribute for their name
char* Discussion::name() {
    if (!attributes["title"].is_string()) return NULL;
    return strdup(attributes["title"].get<std::string>().c_str());
}