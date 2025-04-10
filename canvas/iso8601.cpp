#include "iso8601.hpp"

// Written by ChatGPT, modified by Aaronjamt

#include <iomanip>
#include <sstream>
#include <ctime>

std::string ISO8601::get_day_suffix(int day) {
    if (day >= 11 && day <= 13) return "th"; // Special case for 11-13
    switch (day % 10) {
        case 1: return "st";
        case 2: return "nd";
        case 3: return "rd";
        default: return "th";
    }
}

std::string ISO8601::format_to_human(const std::string& iso8601) {
    struct std::tm tm = {};
    std::istringstream ss(iso8601);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    
    if (ss.fail()) {
        return "Invalid date format";
    }

    // Convert UTC to local time
    time_t utc_time = timegm(&tm);
    struct std::tm local_tm;
    localtime_r(&utc_time, &local_tm); // Convert to local time

    // Create a human-readable format
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%A, %B %e", &local_tm);
    
    std::ostringstream output;
    output << buffer << get_day_suffix(local_tm.tm_mday) 
           << " at " << (local_tm.tm_hour % 12 == 0 ? 12 : local_tm.tm_hour % 12) 
           << ":" << std::setw(2) << std::setfill('0') << local_tm.tm_min 
           << (local_tm.tm_hour >= 12 ? " PM" : " AM");

    return output.str();
}