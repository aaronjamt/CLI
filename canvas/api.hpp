#ifndef _API_HPP
#define _API_HPP

#include <curl/curl.h>
#include "nlohmann/json.hpp"

class CanvasAPI {
	// Constructor and destructor are public
	public:
		CanvasAPI(const char* base_url, const char* token);
		~CanvasAPI();

		nlohmann::json _request(const char *endpoint, bool is_post); //, const char *method, headers=None, data=None

		// Default to GET request
		nlohmann::json _request(const char *endpoint) {
			return _request(endpoint, false);
		};

	// Private members are only accessible to this class
	private:
		const char *base_url;
		CURL *curl;
		CURLcode res;
};

#endif//_API_HPP
