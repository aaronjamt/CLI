#include <stdio.h>
#include "canvas/canvas.hpp"

int main(int argc, char *argv[]) {
	printf("Hello, world!\n");

	if (argc < 3) {
		printf("Please provide a Canvas base URL and API token.\n");
		return 1;
	}
	Canvas canvas(argv[1], argv[2]);

	printf("Hello, %s! Welcome to the Canvas LMS Interface (CLI).\n", canvas.get_current_user().name());
	printf("Here are your courses:\n");
	for (Course course : canvas.get_courses()) {
		printf("> ID #%ld: %s\n", course.id(), course.name());
	}

	// TODO: Use canvas object

	return 0;
}
