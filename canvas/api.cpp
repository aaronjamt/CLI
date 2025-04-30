#include "api.hpp"
#include "nlohmann/json.hpp"
#include "andsonye/multipart_parser.h"

#include <iostream>
#include <string>
#include <cstring>

// Uncomment to enable libcURL verbose output
// This will print all HTTP requests and responses to the console
// #define CURL_VERBOSE

// Uncomment to enable logging HTTP errors
// #define HTTP_VERBOSE

// Class constructor initializes libcURL
CanvasAPI::CanvasAPI(const char *url, const char* token) {
    // Prepare CURL
    curl = curl_easy_init();
    if(curl) {
#ifdef CURL_VERBOSE
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#endif
        // Provide access token for authorization
        curl_easy_setopt(curl, CURLOPT_XOAUTH2_BEARER, token);
    } else {
        throw std::runtime_error("Failed to initialize libcurl.");
    }

    base_url = url;
}

// Class destructor cleans up libcURL
CanvasAPI::~CanvasAPI() {
    curl_easy_cleanup(curl);
}

// Callback function to handle the data received
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
    size_t total_size = size * nmemb;
    std::string *response = (std::string *)data;
    response->append((char *)ptr, total_size);  // Append received data to the response string
    return total_size;
}

// Callback function to handle the headers received
size_t header_callback(void *ptr, size_t size, size_t nmemb, std::string *data) {
    // Check if the header starts with "LINK:", case insensitive
    if (strncasecmp((char *)ptr, "LINK:", 5) == 0) {
        std::string remainder = (char *)ptr + 5;

        int comma_idx;
        do {
            // Split at first comma
            comma_idx = remainder.find(',');
            std::string link = remainder.substr(0, comma_idx);
            remainder = remainder.substr(comma_idx + 1);

            // Split link subsection at first semicolon
            int semicolon_idx = link.find(';');
            std::string url = link.substr(0, semicolon_idx);
            std::string rel = link.substr(semicolon_idx + 1);

            // Check if the link is a "next" link
            if (rel.find("rel=\"next\"") != std::string::npos) {
                // If we found the next page, extract the URL and return early
                int start = url.find('<') + 1;
                int length = url.find('>') - start;
                *data = url.substr(start, length);
                return size * nmemb;
            }
        } while (comma_idx != -1);
    } else if (data->empty()) {
        // Otherwise, if we haven't gotten a next page, check if
        // the header starts with "LOCATION:", case insensitive
        if (strncasecmp((char *)ptr, "LOCATION:", 9) == 0) {
            // If we found the location, extract the URL and return early
            std::string url = (char *)ptr + 9;
            
            // Strip any whitespace
            url.erase(remove_if(url.begin(), url.end(), isspace), url.end());

            // Duplicate (so it doesn't get freed when this function returns) and store the URL
            *data = strdup(url.c_str());
            return size * nmemb;
        }
    }
    return size * nmemb;
}

std::optional<nlohmann::json> CanvasAPI::_requestURL(std::string url, nlohmann::json post_data, std::optional<std::string> file_path, std::string file_param) {
    // Set the URL to request and the authentication method
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);

    // Buffer to store the response body
    std::string response;

    // If there's a next page, store the URL here
    std::string next_page;

    // Set the write function and the data buffer to store the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Set the header function and the data buffer to store the headers
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &next_page);

    struct curl_slist *headers = NULL;

    // Set appropriate HTTP request type
    if (post_data == NULL) {
        // GET request
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);
    } else {
        // POST request
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        // If we have a file path, upload the file
        if (file_path) {
            web::http::MultipartParser parser;

            // Add the post data
            for (auto& [key, value] : post_data.items()) {
                parser.AddParameter(key, value);
            }

            // Add the file
            parser.AddFile(file_param, file_path->c_str());

            // Set the content type
            std::string type = "Content-Type: multipart/form-data; boundary=";
            type.append(parser.boundary());
            headers = curl_slist_append(headers, type.c_str());

            // Add the body
            std::string body = parser.GenBodyContent();
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strdup(body.c_str()));

            // Disable authentication for file uploads
            curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_NONE);
        } else {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strdup(post_data.dump().c_str()));
            headers = curl_slist_append(NULL, "Content-Type: application/json");
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Perform the request, res gets the return code
    res = curl_easy_perform(curl);
    // Check for errors
    if(res != CURLE_OK) {
#ifdef HTTP_VERBOSE
        printf("Error making HTTP %s request to '%s': %s\n", 
            post_data == NULL ? "GET" : "POST",
            url.c_str(),
            curl_easy_strerror(res));
#endif
        return std::nullopt;
    } else {
        long response_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

        // Check for a redirect
        if (response_code >= 300 && response_code < 400) {
            // Check if we got a location header
            if (!next_page.empty()) {
                // The Canvas API is weird. The only time it should return a redirect is after
                // a file upload, and it specifically says to make a GET request, rather
                // than a POST request, even though that would technically be more correct.
                return _requestURL(next_page, NULL, {}, "");
            }
        }

        // HTTP success codes are in the 200 range, check for errors
        if (response_code >= 300) {
#ifdef HTTP_VERBOSE
            printf("Received HTTP response code %ld for URL %s.\n", response_code, url.c_str());
#endif
            return std::nullopt;
        }

        // Check for content-type
        char *content_type;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
        if (content_type == NULL || strncmp(content_type, "application/json", 16) != 0) {
#ifdef HTTP_VERBOSE
            printf("Received invalid content type: %s for URL %s.\n", content_type, url.c_str());
#endif
            return std::nullopt;
        }

        nlohmann::json result = nlohmann::json::parse(response);
        if (!next_page.empty()) {
            // In theory, this should make the same request to the new URL, but it seems like Canvas API always expects an HTTP GET
            std::optional<nlohmann::json> next_result = _requestURL(next_page, NULL, {}, "");
            if (next_result) {
                // Merge the two JSON objects
                if (result.is_array() && next_result->is_array()) {
                    result.insert(result.end(), next_result->begin(), next_result->end());
                } else if (result.is_object() && next_result->is_object()) {
                    result.update(next_result.value());
                } else {
#ifdef HTTP_VERBOSE
                    printf("Cannot merge JSON objects of types %s and %s.\n", result.type_name(), next_result->type_name());
#endif
                    return std::nullopt;
                }
            }
        }

#ifdef HTTP_VERBOSE
        if (file_path) {
            std::cout << result.dump(4) << std::endl;
        }
#endif

        return result;
    }
}
