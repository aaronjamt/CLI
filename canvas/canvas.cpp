#include "canvas.hpp"

std::vector<Course> Canvas::get_courses() {
	nlohmann::json json_response = _request("/api/v1/courses");
	std::vector<Course> courses;
	for (nlohmann::json course : json_response) {
		courses.push_back(Course(this, course));
	}
    return courses;
}

Course Canvas::get_course_by_id(unsigned long course_id) {
    std::string endpoint = "/api/v1/courses/" + std::to_string(course_id);
    nlohmann::json json_response = _request(endpoint.c_str());
    return Course(this, json_response);
}

User Canvas::get_current_user() {
	nlohmann::json json_response = _request("/api/v1/users/self");
	return User(this, json_response);
}
