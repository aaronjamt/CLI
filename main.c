#include <stdio.h>
#include "canvas/canvas.hpp"

int main(int argc, char *argv[]) {
	printf("Hello, world!\n");

	if (argc < 3) {
		printf("Please provide a Canvas base URL and API token.\n");
		return 1;
	}
	Canvas canvas(argv[1], argv[2]);
	printf("\x1b[30;47mHello, %s! Welcome to the Canvas LMS Interface (CLI).\x1b[K\x1b[0m\n", canvas.get_current_user().name());
	printf("Here are your courses:\n");
	for (Course course : canvas.get_courses()) {
		printf("> ID #%ld: %s", course.id(), course.name());
		printf(" | Grade: %.2f%%\n", course.get_final_score());
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
