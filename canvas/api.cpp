#include "api.hpp"
#include "nlohmann/json.hpp"

#include <string>
#include <cstring>

//#define CURL_VERBOSE

// Class constructor initializes libcURL
CanvasAPI::CanvasAPI(const char *url, const char* token) {
	// Prepare CURL
	curl = curl_easy_init();
	if(curl) {
#ifdef CURL_VERBOSE
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
#endif
		// Provide access token for authorization
		curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BEARER);
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
	}
	return size * nmemb;
}

std::optional<nlohmann::json> CanvasAPI::_requestURL(std::string url, nlohmann::json post_data) {
	// Set the URL to request
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

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

	// Set appropriate HTTP request type
	if (post_data == NULL)
		// GET request
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
	else {
		// POST request
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strdup(post_data.dump().c_str()));

		// Set the content type to JSON
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(NULL, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	}

	// Perform the request, res gets the return code
	res = curl_easy_perform(curl);
	// Check for errors
	if(res != CURLE_OK) {
		printf("Error making HTTP %s request to %s: %s\n", 
			post_data == NULL ? "GET" : "POST",
			url.c_str(),
			curl_easy_strerror(res));
		return std::nullopt;
	} else {
		long response_code;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (response_code != 200) {
			printf("Received HTTP response code %ld for URL %s.", response_code, url.c_str());
			return std::nullopt;
		}

		// Check for content-type
		char *content_type;
		curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
		if (content_type == NULL || strncmp(content_type, "application/json", 16) != 0) {
			printf("Received invalid content type: %s for URL %s.", content_type, url.c_str());
			return std::nullopt;
		}

		nlohmann::json result = nlohmann::json::parse(response);
		if (!next_page.empty()) {
			std::optional<nlohmann::json> next_result = _requestURL(next_page, post_data);
			if (next_result.has_value()) {
				// Merge the two JSON objects
				if (result.is_array() && next_result.value().is_array()) {
					result.insert(result.end(), next_result.value().begin(), next_result.value().end());
				} else if (result.is_object() && next_result.value().is_object()) {
					result.update(next_result.value());
				} else {
					printf("Cannot merge JSON objects of types %s and %s.", result.type_name(), next_result.value().type_name());
					return std::nullopt;
				}
			}
		}
		return result;
	}
}
