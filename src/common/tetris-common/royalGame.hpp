#include "classicGame.hpp"

class RoyalGame : public ClassicGame
{
private:
    int timer;
    
public:
    // Malus Method
    void inverted_command(bool flag) override
    {
        reverseControls = flag;
    }

    void block_command() override
    {
    }

    void thunder_strike() override {}
    void fast_falling_pieces() override {}
    void light_off() override {}

    // Bonuses Method
    void blocs_1x1() override {}
    void slow_falling_pieces() override {}
};