#include "canvas.hpp"
#include "api.hpp"
#include <stdexcept>

std::vector<Course> Canvas::get_courses() {
	std::string endpoint = "/api/v1/courses?include[]=total_scores";
	std::optional<nlohmann::json> json_response = _request(endpoint.c_str());
	if (!json_response.has_value()) return {};

	std::vector<Course> courses;
	for (nlohmann::json course : json_response.value()) {
		courses.push_back(Course(this, course));
	}
	return courses;
}

std::optional<Course> Canvas::get_course_by_id(unsigned long course_id) {
	std::string endpoint = "/api/v1/courses/" + std::to_string(course_id) + "?include[]=total_scores";
	std::optional<nlohmann::json> json_response = _request(endpoint.c_str());	if (!json_response.has_value()) return {};

	if (json_response.has_value())
		return Course(this, json_response.value());
	else
		return std::nullopt;
}

std::optional<User> Canvas::get_current_user() {
	std::string endpoint = "/api/v1/users/self";
	std::optional<nlohmann::json> json_response = _request(endpoint.c_str());	if (!json_response.has_value()) return {};

	if (json_response.has_value())
		return User(this, json_response.value());
	else
		return std::nullopt;
}
