#include <stdio.h>
#include "canvas/canvas.hpp"

int main(int argc, char *argv[]) {
	printf("Hello, world!\n");

	if (argc < 2) {
		printf("Please provide a Canvas API token.\n");
		return 1;
	}
	Canvas canvas(argv[1]);

	// TODO: Use canvas object

	return 0;
}
