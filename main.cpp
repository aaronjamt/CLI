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

        std::string announcements;
        for (Discussion announcement : course.announcements()) {
            announcements.append("  > " + (std::string)announcement.name() + "\n");
        }

        formatted_courses.push_back(
            "> ID #" + std::to_string(course.id()) + ": " + course.name() + " | Grade: " + realGradeFuckingForRealThisTime + "\n" +
            std::to_string(course.announcements().size()) + " announcements:\n" + announcements
        );
    }

    for (const std::string &to_c : formatted_courses) {
        course_names.push_back(to_c.c_str());
    }

    int selected_item = menu("Select a course:", course_names.size(), course_names.data());
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
        const char* objectList[100] = {
            "Announcements",
            "Quizzes",
            "Assignments",
            "Discussions",
            "I'm Feeling Lucky"
        };

        int selected_object = menu("Select an object:", 5, objectList);
        if (selected_object == 4) {
            printf("Randomly selecting an object...\n");
            sleep(3);
            selected_object = rand() % 4;
        }
        switch (selected_object) {
            case 0:
                printf("\e[1;1H\e[2J");
                printf("\nAnnouncements for %s:\n", selected_course.name());

                for (Discussion announcement : selected_course.announcements()) {
                    printf("> ID #%ld: '%s' posted by %s at %s", announcement.id(), announcement.name(), announcement.poster_name(), announcement.posted_at());
                    if (announcement.assignment()) {
                        printf(" | Due at: %s", announcement.assignment()->due_date());
                    }
                    printf("  > %s\n", announcement.body());
                }
                break;
            case 1:
                printf("\e[1;1H\e[2J");
                printf("\nQuizzes for %s:\n", selected_course.name());

                for (Quiz quiz : selected_course.quizzes()) {
                    printf("> ID #%ld: %s | Due at: %s\n", quiz.id(), quiz.name(), quiz.due_date());
                }

                printf("Please select a quiz:\n");
                sleep(1);

                if (1) {
                    std::vector<Quiz> quizzes = selected_course.quizzes();
                    std::vector<const char*> quiz_names;
                    for (Quiz quiz : quizzes) {
                        quiz_names.push_back(quiz.name());
                    }

                    int selected_quiz = menu("Select a quiz:", quiz_names.size(), quiz_names.data());
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
                        printf("Quiz submission ID: %ld\n", quiz_submission->id());
                        std::vector<QuizQuestion>& questions = quiz_submission->questions();
                        for (QuizQuestion& question : questions) {
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
                                        printf("\e[1;1H\e[2J%s\n\n", question.name());

                                        double answer;
                                        printf("Enter a number: ");
                                        scanf("%lf", &answer);

                                        if (!question.set_answer(answer)) {
                                            // TODO: Better error handling
                                            printf("Failed to set answer.\n");
                                            sleep(1);
                                        }
                                    }
                                case Text:
                                    {
                                        printf("\e[1;1H\e[2J%s\n\n", question.name());

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
                    sleep(5);
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
                break;
            case 3:
                printf("Discussions for %s:\n", selected_course.name());
                for (Discussion discussion : selected_course.discussions()) {
                    printf("> ID #%ld: '%s' posted by %s at %s", discussion.id(), discussion.name(), discussion.poster_name(), discussion.posted_at());
                    if (discussion.assignment()) {
                        printf(" | Due at: %s", discussion.assignment()->due_date());
                    }
                    printf("  > %s\n", discussion.body());
                }
                break;

        }
    }

    printf("\n");

    /*
    // TODO: File upload not yet working
    Assignment y = selected_course.assignments()[0];
    printf("Upload file returned %s.\n", y.upload_file("./t.txt") ? "success" : "failure");
    */

    return 0;
}
