#ifndef KEYSTROKE_HPP
#define KEYSTROKE_HPP


#include <iostream>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "types.hpp"



struct KeyStrokePacket {
    
    public:

        Action action;
        std::string token;

        [[nodiscard]] std::string serialize() const;
        [[nodiscard]] static KeyStrokePacket deserialize(const std::string& data);

};



#endif