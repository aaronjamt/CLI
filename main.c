#include <stdio.h>
#include "canvas/canvas.hpp"

int main() {
	printf("Hello, world!\n");

	if (argc < 2) {
		printf("Please provide a Canvas API token.\n");
		return 1;
	}
	Canvas canvas = new Canvas();
	if (!canvas.login(argv[1])) {
		printf("Failed to login. Invalid token?\n");
		return 1;
	}

	return 0;
}
