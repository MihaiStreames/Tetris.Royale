#pragma once

#include <map>
#include <ftxui/component/event.hpp>
#include "tetris-common/types.hpp"

class inputHandler {
    std::map<ftxui::Event, Action>  inputMap;
    std::map<ftxui::Event, int>     cooldownedKeys;
    int coolDown;
    ftxui::Event currentEvent;

    static constexpr int SHORT_COOLDOWN = 5;
    static constexpr int LONG_COOLDOWN  = 10;

public:
    inputHandler() : coolDown(0), currentEvent(ftxui::Event::Custom) { setupController(); }

    void setupController() {
        inputMap[ftxui::Event::ArrowLeft]       = MoveLeft;
        inputMap[ftxui::Event::ArrowRight]      = MoveRight;
        inputMap[ftxui::Event::ArrowDown]       = MoveDown;
        inputMap[ftxui::Event::Character('q')]  = RotateRight;
        inputMap[ftxui::Event::Character('e')]  = RotateLeft;
        inputMap[ftxui::Event::Character('b')]  = UseBag;
    }

    static int getActionCooldown(const Action a) {
        switch (a) {
            case MoveLeft:
            case MoveRight:
            case MoveDown:
                return SHORT_COOLDOWN;
            case RotateLeft:
            case RotateRight:
            case UseBag:
                return LONG_COOLDOWN;
            default:
                return SHORT_COOLDOWN;
        }
    }

    void putKeyInCooldown(const ftxui::Event& evt, const Action a) { cooldownedKeys[evt] = getActionCooldown(a); }

    // Decrease cooldowns each frame
    void handleCooldown() {
        std::vector<ftxui::Event> finished;
        for (auto& [evt, cd] : cooldownedKeys) {
            if (cd > 0) {
                cd--;
            }

            if (cd == 0) {
                finished.push_back(evt);
            }
        }

        for (auto evt: finished) {
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
        const Action leftAction = inputMap[ftxui::Event::ArrowLeft];
        inputMap[ftxui::Event::ArrowLeft] = inputMap[ftxui::Event::ArrowRight];
        inputMap[ftxui::Event::ArrowRight] = leftAction;
    }

    [[nodiscard]] int getCoolDown() const   { return coolDown; }
    void setCoolDown(const int val)         { coolDown = val; }

    void setEvent(const ftxui::Event& evt)  { currentEvent = evt; }
};