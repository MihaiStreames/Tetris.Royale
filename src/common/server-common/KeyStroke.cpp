#include "KeyStroke.hpp"


std::string KeyStrokePacket::serialize() const {
    // serialize the key stroke packet to a json string
    nlohmann::json j;

    j["action"] = action;
    j["token"] = token;

    return j.dump();
}


KeyStrokePacket KeyStrokePacket::deserialize(const std::string &data) {
    // deserialize the key stroke packet from a json string
    nlohmann::json j;

    // some try-catch blocks because I'm not lazy anymore (I'm still lazy)
    try {
        j = nlohmann::json::parse(data);
    } catch (nlohmann::json::parse_error &e) {
        throw std::runtime_error(
            "[error] Parsing failed while deserializing KeyStrokePacket: " + std::string(e.what()));
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing KeyStrokePacket: " + std::string(e.what()));
    }

    KeyStrokePacket packet;

    try {
        packet.action = static_cast<KeyStroke>(j["action"].get<int>());
        packet.token = j["token"].get<std::string>();
    } catch (nlohmann::json::exception &e) {
        throw std::runtime_error(
            "[error] Unknown json error while deserializing KeyStrokePacket: " + std::string(e.what()));
    } catch (std::exception &e) {
        throw std::runtime_error("[error] Unknown error while deserializing KeyStrokePacket: " + std::string(e.what()));
    }

    return packet;
}
