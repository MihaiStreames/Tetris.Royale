#pragma once

#include <map>
#include <ftxui/component/event.hpp>
#include "tetris-common/types.hpp"

class inputHandler {
    std::map<Event, Action>  inputMap;
    std::map<Event, int>     cooldownedKeys;
    int cooldown;
    Event currentEvent;

    static constexpr int SHORT_COOLDOWN = 5;
    static constexpr int LONG_COOLDOWN  = 10;

public:
    inputHandler() : cooldown(0), currentEvent(Event::Custom) { setupController(); }

    void setupController() {
        inputMap[Event::ArrowLeft]       = MoveLeft;
        inputMap[Event::ArrowRight]      = MoveRight;
        inputMap[Event::ArrowDown]       = MoveDown;
        inputMap[Event::Character(' ')]  = InstantFall;
        inputMap[Event::Character('q')]  = RotateRight;
        inputMap[Event::Character('e')]  = RotateLeft;
        inputMap[Event::Character('b')]  = UseBag;
    }

    static int getActionCooldown(const Action a) {
        switch (a) {
            case MoveLeft:
            case MoveRight:
            case MoveDown:
            case InstantFall:
                return SHORT_COOLDOWN;
            case RotateLeft:
            case RotateRight:
            case UseBag:
                return LONG_COOLDOWN;
            default:
                return SHORT_COOLDOWN;
        }
    }

    void putKeyInCooldown(const Event& evt, const Action a) { cooldownedKeys[evt] = getActionCooldown(a); }

    // Decrease cooldowns each frame
    void handleCooldown() {
        std::vector<Event> finished;
        for (auto& [evt, cd] : cooldownedKeys) {
            if (cd > 0) {
                cd--;
            }

            if (cd == 0) {
                finished.push_back(evt);
            }
        }

        for (const auto& evt: finished) {
            cooldownedKeys.erase(evt);
        }
    }

    Action getUserAction() {
        if (const auto it = inputMap.find(currentEvent); it != inputMap.end()) {
            const auto& evt = it->first;
            const Action a = it->second;

            // Check if in cooldown
            if (!cooldownedKeys.contains(evt)) {
                // Not in cooldown, put it in now
                putKeyInCooldown(evt, a);
                return a;
            }
        }

        return None;
    }

    Action handleInputs() {
        const Action action = getUserAction();
        handleCooldown();
        return action;
    }

    // Swap the actions for left and right
    void invertKeys() {
        const Action leftAction      = inputMap[Event::ArrowLeft];
        inputMap[Event::ArrowLeft]   = inputMap[Event::ArrowRight];
        inputMap[Event::ArrowRight]  = leftAction;
    }

    [[nodiscard]] int getCoolDown() const   { return cooldown; }
    void setCoolDown(const int val)         { cooldown = val; }

    void setEvent(const Event& evt)  { currentEvent = evt; }
};