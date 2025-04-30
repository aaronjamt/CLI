#include "assignment.hpp"
#include "iso8601.hpp"
#include <iostream>
#include <optional>

std::optional<Submission> Assignment::submission() {
    if (this->attributes.is_object())
        return Submission(api, this, this->attributes["submission"]);
    return std::nullopt;
}

std::optional<double> Assignment::value() {
    if (attributes["points_possible"].is_number()) {
        return attributes["points_possible"].get<double>();
    }
    return std::nullopt;
}

char* Assignment::posted_at() {
    if (attributes["created_at"].is_string()) {
        std::string when = attributes["created_at"].get<std::string>();
        when = ISO8601::format_to_human(when);
        return strdup(when.c_str());
    }
    return (char*)"";
}

char* Assignment::due_date() {
    if (attributes["due_at"].is_string()) {
        std::string when = attributes["due_at"].get<std::string>();
        when = ISO8601::format_to_human(when);
        return strdup(when.c_str());
    }
    return (char*)"";
}

std::optional<File> Assignment::upload_file(const char* path) {
    // Extract the file name from the path
    std::string file_path = path;
    std::string::size_type last_slash = file_path.find_last_of("/");
    if (last_slash == std::string::npos) last_slash = -1;
    std::string file_name = file_path.substr(last_slash + 1);

    // Request to upload the file
    std::string endpoint = "/api/v1/courses/" + std::to_string(course->id()) + "/assignments/" + std::to_string(id()) + "/submissions/self/files";
    std::optional<nlohmann::json> response = api->_request(endpoint.c_str(), {
        {"name", file_name},
        {"size", std::filesystem::file_size(path)}
    });
    if (!response || !response.value().contains("upload_url")) {
        return {};
    }

    // Fetch the URL to actually upload to
    std::string upload_url = response.value()["upload_url"];
    if (!upload_url.empty()) {
        // Fetch any upload parameters
        if (!response.value().contains("upload_params")) {
            return {};
        }
        nlohmann::json upload_params = response.value()["upload_params"];

        std::string file_param = "file";
        if (response.value().contains("file_param") && response.value()["file_param"].is_string()) {
            file_param = response.value()["file_param"];
        }

        // Perform the upload
        std::optional<nlohmann::json> upload_response = api->_requestURL(upload_url, upload_params, path, file_param);
        if (upload_response) {
            return File(api, upload_response.value());
        }
    }

    return {};
}
