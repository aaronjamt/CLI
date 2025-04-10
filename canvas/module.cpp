#include "module.hpp"

std::string Module::name() {
    return attributes["name"];
}

long Module::id() {
    return attributes["id"];
}

std::vector<ModuleItem> Module::items() {
    std::string endpoint = attributes["items_url"];
    std::optional<nlohmann::json> json_response = api->_request(endpoint.c_str());
    if (!json_response) return {};

    std::vector<ModuleItem> result;
    for (auto &item : json_response.value()) {
        result.emplace_back(api, item);
    }
    return result;
}