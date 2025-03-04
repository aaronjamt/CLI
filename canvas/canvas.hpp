#ifndef _CANVAS_HPP
#define _CANVAS_HPP

#include <stdexcept>
#include <curl/curl.h>

class Course {
	unsigned long id;
	char *name;
	char *teacher;
	float grade;
};

class Canvas {
	public:
	Canvas(const char* base_url, const char* token);
	~Canvas();
//	Course get_course_by_id(unsigned long course_id);

	private:
	const char *base_url;
	CURL *curl;
	CURLcode res;

	bool _request(const char *endpoint, bool is_post); //, const char *method, headers=None, data=None

	// Default to GET request
	bool _request(const char *endpoint) {
		return _request(endpoint, false);
	};
};

#endif//_CANVAS_HPP
