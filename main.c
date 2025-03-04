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

	printf("\n");

	Course course = canvas.get_courses()[0];
	printf("Quizzes for %s:\n", course.name());
	for (Quiz quiz : course.quizzes()) {
		printf("> ID #%ld: %s\n", quiz.id(), quiz.name());
	}

	printf("\n");

	printf("Assignments for %s:\n", course.name());
	for (Assignment assignment : course.assignments()) {
		printf("> ID #%ld: %s\n", assignment.id(), assignment.name());
	}

	// TODO: Use canvas object

	return 0;
}
