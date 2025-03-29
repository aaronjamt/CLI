#ifndef _ISO8601_HPP
#define _ISO8601_HPP

#include <string>
#include <ctime>

class ISO8601 {
    public:
        static std::string format_to_human(const std::string& iso8601);

    private:
        static std::string get_day_suffix(int day);
};

#endif//_ISO8601_HPPs