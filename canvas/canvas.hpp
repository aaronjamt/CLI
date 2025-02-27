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
	Canvas(const char* token);
	~Canvas();
//	Course get_course_by_id(unsigned long course_id);

	private:
	CURL *curl;
	CURLcode res;

	bool _request(const char *endpoint); //, const char *method, headers=None, data=None
};

#endif//_CANVAS_HPP
