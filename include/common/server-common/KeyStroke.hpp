#ifndef KEYSTROKE_HPP
#define KEYSTROKE_HPP

#include "Types.hpp"

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

struct KeyStrokePacket
{
  public:
    Action action;
    std::string token;

    [[nodiscard]] std::string serialize() const;
    [[nodiscard]] static KeyStrokePacket deserialize(const std::string& data);
};

#endif