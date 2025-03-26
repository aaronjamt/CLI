#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "canvas/canvas.hpp"
#include "canvas/submission.hpp"

#define UP_ARROW 65
#define DOWN_ARROW 66
#define ENTER 10

int get_key_press() {
    struct termios oldt, newt;
    int ch;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt; // Save terminal settings
    newt.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();

    if (ch == 27) {  // Escape sequence for arrow keys
        if (getchar() == '[') ch = getchar();
        else ch = 0;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
int menu(int num_items, const char *items[]) {
    int selected_item = 0;
    while (1) {
        printf("\e[1;1H\e[2J");
        for (int i = 0; i < num_items; i++) {
            if (selected_item == i) printf("\x1b[30;47m");
            printf("%s\n", items[i]);
            if (selected_item == i) printf("\x1b[37;40m");
        }
        int input = get_key_press();
        if (input == UP_ARROW) {
            selected_item--;
            if (selected_item < 0) selected_item = num_items - 1;
        } else if (input == DOWN_ARROW) {
            selected_item++;
            if (selected_item >= num_items) selected_item = 0;
        } else if (input == ENTER) {
            return selected_item;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Please provide a Canvas base URL and API token.\n");
        return 1;
    }
    Canvas canvas(argv[1], argv[2]);
    printf("\x1b[30;47mHello, %s! Welcome to the Canvas LMS Interface (CLI).\x1b[K\x1b[0m\n", canvas.get_current_user()->name());
    printf("Here are your courses:\n");
    std::vector<Course> courses = canvas.get_courses();
    std::vector<std::string> formatted_courses;
    std::vector<const char *> course_names;

    for (Course course : courses) {
        std::string realGradeFuckingForRealThisTime;
        if (course.get_current_grade().has_value()) {
            realGradeFuckingForRealThisTime = std::to_string(course.get_current_grade().value()) + "%";
        } else {
            realGradeFuckingForRealThisTime = "N/A";
        }
        formatted_courses.push_back("> ID #" + std::to_string(course.id()) + ": " + course.name() + " | Grade: " + realGradeFuckingForRealThisTime);
    }

    for (const std::string &to_c : formatted_courses) {
        course_names.push_back(to_c.c_str());
    }

    int selected_item = menu(course_names.size(), course_names.data());
    Course selected_course = courses[selected_item];

    printf("\e[1;1H\e[2J");

    if (selected_course.get_current_grade().has_value()) {
        printf("Selected course: ID #%ld: %s | Grade: %.2f%%\n", selected_course.id(), selected_course.name(), selected_course.get_current_grade().value());
    } else {
        printf("Selected course: ID #%ld: %s | Grade: N/A\n", selected_course.id(), selected_course.name());
    }

    printf("\n");
    srand(time(NULL));
    const char* objectList[100] = {
        "Announcements",
        "Quizzes",
        "Assignments",
        "Discussions",
        "I'm Feeling Lucky"
    };

    int selected_object = menu(5, objectList);
    if (selected_object == 4) {
        printf("Randomly selecting an object...\n");
        sleep(3);
        selected_object = rand() % 4;
    }
    switch (selected_object) {
        case 0:
            printf("\e[1;1H\e[2J");
            printf("\nAnnouncements for %s:\n", selected_course.name());
            break;
        case 1:
            printf("\e[1;1H\e[2J");
            printf("\nQuizzes for %s:\n", selected_course.name());
            for (Quiz quiz : selected_course.quizzes()) {
                printf("> ID #%ld: %s\n", quiz.id(), quiz.name());
        }
            break;
        case 2:
            printf("\e[1;1H\e[2J");
            printf("\nAssignments for %s:\n", selected_course.name());
            for (Assignment assignment : selected_course.assignments()) {
                printf("> ID #%ld: %s", assignment.id(), assignment.name());
            
                // TODO: add due date function

                // if (assignment.due_date())
                //     printf("\nDue date: %s\n", assignment.due_date().value().c_str());
                // else
                //     printf("Due date: N/A\n");
    
                if (assignment.submission()) {
                    Submission submission = assignment.submission().value();
                    if (submission.score())
                        printf(" | Grade: %.2f%%\n", submission.score().value());
                    else
                        printf(" | Grade: N/A\n");
                } else
                    printf("\n");
            }
            break;
        case 3:
            printf("Discussions for %s:\n", selected_course.name());
            for (Discussion discussion : selected_course.discussions()) {
                printf("Posted by %s\n", discussion.poster(), discussion.name());
            }
            break;

    }

    printf("\n");

    // TODO: Use canvas object
    return 0;
}
