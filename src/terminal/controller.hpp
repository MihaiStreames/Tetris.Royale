#pragma once

#include <map>
#include <ftxui/component/event.hpp>
#include "tetris-common/types.hpp"

class Controller {
    std::map<ftxui::Event, Action>  inputMap;
    std::map<ftxui::Event, int>     invertedKeysCooldown;
    int coolDown;
    ftxui::Event currentEvent;

public:
    Controller() : coolDown(0) { };

    void setupController() {
        inputMap[ftxui::Event::ArrowLeft]       = MoveLeft;
        inputMap[ftxui::Event::ArrowRight]      = MoveRight;
        inputMap[ftxui::Event::ArrowDown]       = MoveDown;
        inputMap[ftxui::Event::Character('q')]  = RotateRight;
        inputMap[ftxui::Event::Character('e')]  = RotateLeft;
        inputMap[ftxui::Event::Character('b')]  = UseBag;
    }

    [[nodiscard]] Action getInput() const {
        if (const auto it = inputMap.find(currentEvent); it != inputMap.end()) {
            return it->second;
        }

        return None;
    }

    void invertedKeys() {
        const Action leftAction = inputMap[ftxui::Event::ArrowLeft];
        inputMap[ftxui::Event::ArrowLeft] = inputMap[ftxui::Event::ArrowRight];
        inputMap[ftxui::Event::ArrowRight] = leftAction;
    }

    [[nodiscard]] int getCoolDown() const   { return coolDown; }
    void setCoolDown(const int val)         { coolDown = val; }

    void setEvent(const ftxui::Event& evt)  { currentEvent = evt; }
};