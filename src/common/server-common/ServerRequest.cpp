#include "ServerRequest.hpp"

std::string
ServerRequest::serialize() const {
    // serialize the request to a json string
    nlohmann::json j;

    j["id"] = id;
    j["method"] = method;
    j["params"] = params;

    return j.dump();
}

ServerRequest
ServerRequest::deserialize(const std::string &data) {
    // deserialize the request from a json string
    nlohmann::json j;

    // some try-catch blocks because I'm not lazy anymore (I'm still lazy)
    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error(
            "[error] Parsing failed while parsing Request: " +
            std::string(e.what()));
    }
    catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while parsing Request: " +
            std::string(e.what()));
    }

    ServerRequest request;

    try {
        request.id = j["id"];
        request.method = static_cast<ServerMethods>(j["method"]);
        request.params =
                j["params"].get<std::unordered_map<std::string, std::string> >();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing Request: " +
            std::string(e.what()));
    }
    catch (std::exception &e) {
        throw std::runtime_error(
            "[error] Unknown error while deserializing Request: " +
            std::string(e.what()));
    }

    return request;
}
