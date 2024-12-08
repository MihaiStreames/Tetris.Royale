#pragma once

#include <map>
#include <ftxui/component/event.hpp>
#include "common.hpp"

class Controller {
    std::map<ftxui::Event, Action>  inputMap;
    std::map<ftxui::Event, int>     invertedKeysCooldown;
    int coolDown;
    ftxui::Event currentEvent;

public:
    Controller();
    void setupController();
    [[nodiscard]] Action getInput() const;
    void invertedKeys();

    [[nodiscard]] int getCoolDown() const   { return coolDown; }
    void setCoolDown(const int val)         { coolDown = val; }

    void setEvent(const ftxui::Event& evt)  { currentEvent = evt; }
};