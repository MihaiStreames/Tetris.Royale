#include "types.hpp"
#include <map>
#include <vector>
#include <chrono>

class PowerUps {
protected:
    // std::vector<TypePowerUps> activePowers; -> dans la gameEngine
    int duration;
    int timeLeft;

public:
    PowerUps(int duration): duration{duration}, timeLeft{duration} {}
    virtual ~PowerUps() {}

    virtual void applyEffect()=0;
    virtual void removeEffect()=0;

    void updateTimer(int deltaTime) {
        timeLeft -= deltaTime;
    }

    bool isExpired() { return timeLeft <= 0;}
};

class InvertedCommand : public PowerUps {
public:
    InvertedCommand(): PowerUps(3000) {} // 3000 -> 3 sec

    void applyEffect() override {
        
    }
};