# Files for main C program
SRC = main.c
# Files for Canvas library
LIB_FILES = canvas/api.cpp canvas/assignment.cpp canvas/discussion.cpp canvas/canvas.cpp canvas/course.cpp canvas/module.cpp canvas/module_item.cpp canvas/quiz.cpp canvas/submission.cpp canvas/iso8601.cpp canvas/andsonye/multipart_parser.cpp

# Name of compiled application
EXC_NAME = cli.out

# Compile as C++
CC = g++
CFLAGS = -g -std=c++23 -Wall -lcurl -I canvas

# This default access token is for a private self-hosted Canvas instance. Don't bother trying to hack me.
TOKEN ?= 83M9TV3fvCheJQMPCVcJYRYWeJ9QTu8cWKRu6m2ZZE7ryXPukNzCuEChKaDVErUC
BASE_URL ?= https://canvas.tail7fcea.ts.net

# When running make with no arguments, build and run
run: build
	@./$(EXC_NAME) $(BASE_URL) $(TOKEN)

# When running make debug, build and run with GDB
debug: build
	@gdb -ex run --args ./$(EXC_NAME) $(BASE_URL) $(TOKEN)

# When running make build, just build
build:$(EXC_NAME)


# Below here handles actually compiling the project


OBJ_FILES = $(LIB_FILES:.cpp=.o) $(SRC:.c=.o)
OBJ = $(patsubst %, %, $(OBJ_FILES))

$(EXC_NAME):$(OBJ)
	$(CC) $(OBJ) -o $(EXC_NAME) $(CFLAGS)

%.o:%.cpp %.hpp
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ) $(EXC_NAME)
