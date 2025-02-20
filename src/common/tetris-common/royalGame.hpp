#include "classicGame.hpp"

class RoyalGame : public ClassicGame
{
private:
    int timer;
    
public:
    // Malus Method
    void inverted_command()
    {
        reverseControls = !reverseControls;
        // Time (30sec)
    }

    void block_command()
    {
    }

    void thunder_strike() {}
    void fast_falling_pieces() {}
    void light_off() {}


    // Bonuses Method
    void blocs_1x1() {}
    void slow_falling_pieces() {}
};