#include "canvas.hpp"

double Course::get_final_score() {
    std::string endpoint = "/api/v1/courses/" + std::to_string(id()) + "/enrollments";
    nlohmann::json json_response = api->_request(endpoint.c_str());

    for (const auto& enrollment : json_response) {
        if (enrollment.contains("grades") && enrollment["grades"].contains("current_score") && enrollment["grades"]["current_score"].is_number()) { 
           return enrollment["grades"]["current_score"].get<double>();
        }
    }
    return 0.0;  // Default if no grade is found
}
