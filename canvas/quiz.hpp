#ifndef _QUIZ_HPP
#define _QUIZ_HPP

#include "nlohmann/json.hpp"
#include "object.hpp"
#include <vector>

// Forward declarations
class Quiz;
class QuizSubmission;
class QuizQuestion;

class Quiz : public CanvasObject {
    public:
        Quiz(CanvasAPI* canvas, CanvasObject *course, nlohmann::json attributes) : CanvasObject(canvas, attributes), course(course) {}

        char *name();
        bool validate_access_code(char* code);
        bool has_access_code();

        std::optional<QuizSubmission> start_quiz(char *access_code = NULL);
        std::vector<QuizQuestion> questions(std::optional<QuizSubmission> submission);

        char* posted_at();
        char* due_date();

    private:
        CanvasObject *course;
};

class QuizSubmission : public CanvasObject {
    public:
        QuizSubmission(CanvasAPI* canvas, CanvasObject *course, long quiz_id, char *access_code, nlohmann::json attributes) : CanvasObject(canvas, attributes), course(course), quiz_id(quiz_id), access_code(access_code) {}

        std::optional<int> attempt();

        std::vector<QuizQuestion>& questions();
        bool update_answers();
        bool submit();
        std::vector<QuizQuestion> _questions;

    private:
        CanvasObject *course;
        long quiz_id;
        char *access_code;
};

typedef enum QuizQuestionType {
    MultipleChoice,
    TrueFalse,
    Number,
    Text,
    Empty
} QuizQuestionType;

class QuizQuestion : public CanvasObject {
    public:
        QuizQuestion(CanvasAPI* canvas, nlohmann::json attributes) : CanvasObject(canvas, attributes) {}

        char *name();

        nlohmann::json get_answers_json();
        std::vector<std::string> get_answers();
        
        bool set_answer(int answer);
        bool set_answer(char* answer);

        QuizQuestionType type();

        std::optional<long> answer();
};

#endif//_QUIZ_HPP