#include <stdio.h>
#include "canvas/canvas.hpp"

int main(int argc, char *argv[]) {
	printf("Hello, world!\n");

	if (argc < 3) {
		printf("Please provide a Canvas base URL and API token.\n");
		return 1;
	}
	Canvas canvas(argv[1], argv[2]);

	// TODO: Use canvas object

	return 0;
}
