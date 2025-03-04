#include "canvas.hpp"
#include "nlohmann/json.hpp"

#include <iostream>
#include <string>
#include <cstring>

//#define CURL_VERBOSE

// Class constructor initializes libcURL
Canvas::Canvas(const char *url, const char* token) {
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

	_request("/api/v1/courses");
}

// Class destructor cleans up libcURL
Canvas::~Canvas() {
	curl_easy_cleanup(curl);
}

// Callback function to handle the data received
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
    size_t total_size = size * nmemb;
    std::string *response = (std::string *)data;
    response->append((char *)ptr, total_size);  // Append received data to the response string
    return total_size;
}

bool Canvas::_request(const char *endpoint, bool is_post) {
	// Combine the base URL and endpoint to form the full URL
	char *url = (char*)malloc(strlen(base_url) + strlen(endpoint) + 1);
	sprintf(url, "%s%s", base_url, endpoint);
	curl_easy_setopt(curl, CURLOPT_URL, url);

	// Buffer to store the response body
	std::string response;

	// Set the write function and the data buffer to store the response
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

	// Set appropriate HTTP request type
	if (is_post)
		curl_easy_setopt(curl, CURLOPT_POST, 1);
	else
		curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);

	// Perform the request, res gets the return code
	res = curl_easy_perform(curl);
	// Check for errors
	if(res != CURLE_OK)
		throw std::runtime_error(curl_easy_strerror(res));
	else {
		printf("Got response from the server!\n");

		auto json_response = nlohmann::json::parse(response);
		std::cout << "First course name: " << json_response[0]["name"] << std::endl;
	}

	return 0;
}
