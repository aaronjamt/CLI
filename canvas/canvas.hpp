#ifndef _CANVAS_HPP
#define _CANVAS_HPP

#include "api.hpp"
#include "announcement.hpp"
#include "assignment.hpp"
#include "comment.hpp"
#include "course.hpp"
#include "discussion.hpp"
#include "file.hpp"
#include "quiz.hpp"
#include "user.hpp"

#include <optional>

// Main Canvas class
class Canvas : public CanvasAPI {
    public:
        Canvas(const char* base_url, const char* token) : CanvasAPI(base_url, token) {}

        std::vector<Course> get_courses();
        std::optional<Course> get_course_by_id(unsigned long course_id);
        std::optional<User> get_current_user();
};

#endif//_CANVAS_HPP