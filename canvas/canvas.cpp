#include "canvas.hpp"
#include "api.hpp"
#include <stdexcept>

std::vector<Course> Canvas::get_courses() {
	try {
		nlohmann::json json_response = _request("/api/v1/courses?include[]=total_scores");
		std::vector<Course> courses;
		for (nlohmann::json course : json_response) {
			courses.push_back(Course(this, course));
		}
		return courses;
	} catch (std::runtime_error) {
		return {};
	}
}

Course Canvas::get_course_by_id(unsigned long course_id) {
    std::string endpoint = "/api/v1/courses/" + std::to_string(course_id) + "?include[]=total_scores";
    nlohmann::json json_response = _request(endpoint.c_str());
    return Course(this, json_response);
}

User Canvas::get_current_user() {
	nlohmann::json json_response = _request("/api/v1/users/self");
	return User(this, json_response);
}
