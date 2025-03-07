#include "quiz.hpp"

#include <iostream>

// Quizzes use the "title" attribute for their name
char* Quiz::name() {
    if (!attributes["title"].is_string()) return NULL;
    return strdup(attributes["title"].get<std::string>().c_str());
}