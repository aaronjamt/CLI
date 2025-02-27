#include "canvas.hpp"

//#define CURL_VERBOSE

// Class constructor initializes libcURL
Canvas::Canvas(const char* token) {
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

	_request("/api/v1/courses");
}

// Class destructor cleans up libcURL
Canvas::~Canvas() {
	curl_easy_cleanup(curl);
}

bool Canvas::_request(const char *endpoint) {
	curl_easy_setopt(curl, CURLOPT_URL, "https://canvas.tail7fcea.ts.net/api/v1/courses");

	/* Perform the request, res gets the return code */
	res = curl_easy_perform(curl);
	/* Check for errors */
	if(res != CURLE_OK)
		throw std::runtime_error(curl_easy_strerror(res));
	else {
		char *ct;
		/* ask for the content-type */
		res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

		if((CURLE_OK == res) && ct)
			printf("We received Content-Type: %s\n", ct);
	}

	return 0;
}
