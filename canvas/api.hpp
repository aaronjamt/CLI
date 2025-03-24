#ifndef _API_HPP
#define _API_HPP

#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include <optional>

class CanvasAPI {
    // Constructor and destructor are public
    public:
        CanvasAPI(const char* base_url, const char* token);
        ~CanvasAPI();

        std::optional<nlohmann::json> _requestURL(std::string url, nlohmann::json post_data);

        // Combine the endpoint with the base URL
        std::optional<nlohmann::json> _request(const char *endpoint, nlohmann::json post_data) {
            // The URL should always be "{base_url}/api/v1/{endpoint}"
            std::string url = base_url;
            // Ensure the URL ends with a slash, before adding the "api/v1/" part
            if (url.back() != '/') url += '/';
            url += "api/v1";
            // If the endpoint includes "api/v1/", don't add it again
            if (std::string(endpoint).find("api/v1/") == std::string::npos) {
                // Add a slash between the base URL and the endpoint if needed
                if (endpoint[0] != '/') url += '/';
                url += endpoint;
            } else {
                url += endpoint + strlen("api/v1") + std::string(endpoint).find("api/v1");
            }
            return _requestURL(url, post_data);
        };

        // GET request has no data
        std::optional<nlohmann::json> _request(const char *endpoint) {
            return _request(endpoint, NULL);
        };

    // Private members are only accessible to this class
    private:
        const char *base_url;
        CURL *curl;
        CURLcode res;
};

#endif//_API_HPP
