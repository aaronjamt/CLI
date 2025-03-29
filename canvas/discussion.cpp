#include "discussion.hpp"
#include "iso8601.hpp"

// Discussions use the "title" attribute for their name
char* Discussion::name() {
    if (!attributes["title"].is_string()) return NULL;
    return strdup(attributes["title"].get<std::string>().c_str());
}
char* Discussion::posted_at() {
    if (attributes["posted_at"].is_string()) {
        std::string when = attributes["posted_at"].get<std::string>();
        when = ISO8601::format_to_human(when);
        return strdup(when.c_str());
    }
    return (char*)"";
}
