class Course {
    unsigned long id;
    char *name;
    char *teacher;
    float grade;
}

class Canvas {
    bool login(char *token);
    Course get_course_by_id(unsigned long course_id);
}