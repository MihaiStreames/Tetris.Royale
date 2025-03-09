#ifndef KEYSTROKE_HPP
#define KEYSTROKE_HPP


#include <iostream>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>


// some actions that are possible in the game
// TODO : support for more actions (if needed) (will be needed)

enum class KeyStroke {
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_DOWN,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    DROP,
    HOLD,
    USE_BAG,
    NONE,  // default value
};


struct KeyStrokePacket {
    
    public:

        KeyStroke action;
        std::string token;

        [[nodiscard]] std::string serialize() const;
        [[nodiscard]] static KeyStrokePacket deserialize(const std::string& data);

};



#endif