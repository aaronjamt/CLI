#include <stdio.h>
#include "canvas/canvas.hpp"

int main(int argc, char *argv[]) {
	printf("Hello, world!\n");

	if (argc < 2) {
		printf("Please provide a Canvas API token.\n");
		return 1;
	}
	Canvas canvas;
	if (!canvas.login("")) {
		printf("Failed to login. Invalid token?\n");
		return 1;
	}

	return 0;
}
