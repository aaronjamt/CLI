#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "canvas/canvas.hpp"
#include "canvas/quiz.hpp"
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

int menu(const char *header, int num_items, const char *items[]) {
    // Hide the cursor
    printf("\e[?25l");

    int selected_item = 0;
    while (1) {
        printf("\e[1;1H\e[2J%s\n\n", header);
        for (int i = 0; i < num_items; i++) {
            if (selected_item == i) printf("\x1b[7m");
            printf("%s\n", items[i]);
            if (selected_item == i) printf("\x1b[0m");
        }
        int input = get_key_press();
        if (input == UP_ARROW) {
            selected_item--;
            if (selected_item < 0) selected_item = num_items - 1;
        } else if (input == DOWN_ARROW) {
            selected_item++;
            if (selected_item >= num_items) selected_item = 0;
        } else if (input == ENTER) {
            // Show the cursor before exiting
            printf("\e[?25h");

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

    const char *course_names[100];
    int num_courses=0;

    printf("\e[1;1H\e[2J");
    printf("\x1b[30;47mHello, %s! Welcome to the Canvas LMS Interface (CLI).\x1b[K\x1b[0m\n", canvas.get_current_user()->name());
    printf("Fetching all courses from Canvas (this may take a bit)...\n");
    std::vector<Course> courses = canvas.get_courses();

    for (Course course : courses) {
        std::string realGradeFuckingForRealThisTime;
        if (course.get_current_grade().has_value()) {
            realGradeFuckingForRealThisTime = std::to_string(course.get_current_grade().value()) + "%";
        } else {
            realGradeFuckingForRealThisTime = "N/A";
        }

        char announcements[1000] = {0};
        int num_announcements=0;
        int announcements_idx=0;
        for (Discussion announcement : course.announcements()) {
            announcements_idx += sprintf(announcements+announcements_idx, "  > %s\n", announcement.name());
        }

        char course_info[1000];
        sprintf(course_info, "> ID #%ld: %s | Grade: %s\n%d announcements:\n%s", course.id(), course.name(), realGradeFuckingForRealThisTime.c_str(), num_announcements, announcements);
        printf("%s\n", course_info);
        course_names[num_courses++] = strdup(course_info);
    }

    int selected_item = menu("Select a course:", num_courses, course_names);
    Course selected_course = courses[selected_item];

    printf("\e[1;1H\e[2J");

    if (selected_course.get_current_grade().has_value()) {
        printf("Selected course: ID #%ld: %s | Grade: %.2f%%\n", selected_course.id(), selected_course.name(), selected_course.get_current_grade().value());
    } else {
        printf("Selected course: ID #%ld: %s | Grade: N/A\n", selected_course.id(), selected_course.name());
    }

    printf("\n");
    srand(time(NULL));
    while (1) {
        const char* objectList[] = {
            "Back",
            "Announcements",
            "Quizzes",
            "Assignments",
            "Discussions",
            "Modules",
            "I'm Feeling Lucky"
        };

        int selected_object = menu("Select an object:", sizeof(objectList)/sizeof(objectList[0]), objectList);
        if (selected_object-- == 0) break;

        if (selected_object == 5) {
            printf("Randomly selecting an object...\n");
            sleep(3);
            selected_object = rand() % 5;
        }
        switch (selected_object) {
            case 0:
                printf("\e[1;1H\e[2J");
                printf("\nAnnouncements for %s:\n", selected_course.name());

                for (Discussion announcement : selected_course.announcements()) {
                    printf("> ID #%ld: '%s' posted by %s at %s\n", announcement.id(), announcement.name(), announcement.poster_name(), announcement.posted_at());
                    // TODO: Not working yet
                    // if (announcement.assignment()) {
                    //     printf(" | Due at: %s", announcement.assignment()->due_date());
                    // }
                    printf("  > %s\n", announcement.body());
                }
                break;
            case 1:
                printf("\e[1;1H\e[2J");
                printf("\nQuizzes for %s:\n", selected_course.name());

                for (Quiz quiz : selected_course.quizzes()) {
                    printf("> ID #%ld: %s | Due at: %s\n", quiz.id(), quiz.name(), quiz.due_date());
                }

                printf("\nPress Enter to continue.");
                while (getchar() != '\n');

                if (1) {
                    std::vector<Quiz> quizzes = selected_course.quizzes();
                    std::vector<const char*> quiz_names;
                    quiz_names.push_back("Back");
                    for (Quiz quiz : quizzes) {
                        quiz_names.push_back(quiz.name());
                    }

                    int selected_quiz = menu("Select a quiz:", quiz_names.size(), quiz_names.data());
                    if (selected_quiz-- == 0) break;

                    Quiz quiz = quizzes[selected_quiz];
                    std::optional<QuizSubmission> quiz_submission;

                    if (quiz.has_access_code()) {
                        printf("This quiz requires an access code. Please enter it now.\n");
                        while (1) {
                            char access_code[200];
                            scanf("%s", access_code);
                            if (!quiz.validate_access_code(access_code)) {
                                printf("Invalid access code. Please try again: ");
                            } else {
                                printf("Starting quiz...\n");
                                quiz_submission = quiz.start_quiz(access_code);
                                break;
                            }
                        }
                    } else {
                        // Start without an access code if one is not required
                        printf("Starting quiz...\n");
                        quiz_submission = quiz.start_quiz();
                    }
                    sleep(1);

                    if (quiz_submission) {
                        std::vector<QuizQuestion>& questions = quiz_submission->questions();
                        printf("There are %ld questions in this quiz.\n", questions.size());
                        printf("Press Enter to continue...\n");
                        while (getchar() != '\n');
                        for (QuizQuestion& question : questions) {
                            printf("Question '%s' type is %d\n", question.name(), question.type());
                            switch (question.type()) {
                                case MultipleChoice:
                                case TrueFalse:
                                    {
                                        const char* answersList[200];
                                        int i = 0;
                                        for (std::string answer : question.get_answers()) {
                                            answersList[i++] = strdup(answer.c_str());
                                        }

                                        int selected_answer = menu(question.name(), i, answersList);
                                        if (!question.set_answer(selected_answer)) {
                                            // TODO: Better error handling
                                            printf("Failed to set answer.\n");
                                            sleep(1);
                                        }
                                        break;
                                    }
                                case Number:
                                    {
                                        printf("\e[1;1H\e[2J%s\n\n# ", question.name());

                                        double answer;
                                        scanf("%lf", &answer);

                                        if (!question.set_answer(answer)) {
                                            // TODO: Better error handling
                                            printf("Failed to set answer.\n");
                                            sleep(1);
                                        }
                                    }
                                case Text:
                                    {
                                        printf("\e[1;1H\e[2J%s\n\n> ", question.name());

                                        char answer[1024];
                                        fgets(answer, 1024, stdin);
                                        if (!question.set_answer(answer)) {
                                            // TODO: Better error handling
                                            printf("Failed to set answer.\n");
                                            sleep(1);
                                        }
                                        break;
                                    }
                                default:
                                    {
                                        printf("\e[1;1H\e[2J%s\n\n", question.name());
                                        // Wait until the user hits enter to continue
                                        printf("No answer needed for this question. Press enter to continue.\n");
                                        scanf("%s", NULL);
                                        break;
                                    }
                            }
                        }
                        printf("\n\nUploading selected answers...\n");
                        if (quiz_submission->update_answers()) {
                            printf("Answers updated successfully. Submitting quiz...\n");
                            if (quiz_submission->submit()) {
                                printf("Quiz submitted successfully.\n");
                            } else {
                                printf("Failed to submit quiz.\n");
                            }
                        } else {
                            printf("Failed to update answers.\n");
                        }
                    } else printf("Invalid quiz_submission.\n");
                }
                break;
            case 2:
                printf("\e[1;1H\e[2J");
                printf("\nAssignments for %s:\n", selected_course.name());
                for (Assignment assignment : selected_course.assignments()) {
                    printf("> ID #%ld: %s | Due at: %s", assignment.id(), assignment.name(), assignment.due_date());
                
                    // TODO: add due date function

                    // if (assignment.due_date())
                    //     printf("\nDue date: %s\n", assignment.due_date().value().c_str());
                    // else
                    //     printf("Due date: N/A\n");
        
                    if (assignment.submission()) {
                        Submission submission = assignment.submission().value();
                        if (submission.score())
                            printf(" | Grade: %.2f%%", submission.score().value());
                        else
                            printf(" | Grade: N/A");
                    }
                    printf("\n");
                }
                printf("\nPress Enter to continue...\n");
                while (getchar() != '\n');

                printf("Upload file returned %s.\n", 

                    selected_course.assignments()[0].upload_file("./t.txt")
                    
                ? "success" : "failure");
                break;
            case 3:
                printf("\e[1;1H\e[2J");
                printf("Discussions for %s:\n", selected_course.name());
                for (Discussion discussion : selected_course.discussions()) {
                    printf("> ID #%ld: '%s' posted by %s at %s\n", discussion.id(), discussion.name(), discussion.poster_name(), discussion.posted_at());
                    // TODO: Not working yet
                    // if (discussion.assignment()) {
                    //     printf(" | Due at: %s", discussion.assignment()->due_date());
                    // }
                    printf("  > %s\n", discussion.body());
                }
                break;
            case 4:
                printf("\e[1;1H\e[2J");
                std::vector<Module> modules = selected_course.modules();
                std::vector<const char*> module_titles;
                std::vector<std::string> temp_titles;

                for (Module &module : modules) {
                    temp_titles.push_back(module.name());
                }
                for (auto &s : temp_titles) module_titles.push_back(s.c_str()); // thanks copilot

                int selected_module_index = menu("Select a module: ", module_titles.size(), module_titles.data());
                Module selected_module = modules[selected_module_index];
                printf("\e[1;1H\e[2J");

                printf("Module: %s\n", selected_module.name().c_str());
                for (ModuleItem &item : selected_module.items()) {
                    printf("    > [%s] %s\n", item.type().c_str(), item.title().c_str());
                break;
            }
        }

        printf("Press Enter to continue...\n");
        while (getchar() != '\n');
    }

    printf("\n");

    return 0;
}
