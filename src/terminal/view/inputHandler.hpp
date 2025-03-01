#pragma once

#include <map>
#include <ftxui/component/event.hpp>
#include "../common/types.hpp"

class inputHandler {
    std::map<ftxui::Event, Action>  inputMap;
    ftxui::Event currentEvent;

public:
    inputHandler() : currentEvent(ftxui::Event::Custom) { setupController(); }

    void setupController();

    Action getUserAction();

    Action handleInputs();

    // Swap the actions for left and right
    void invertKeys();

    void setEvent(const ftxui::Event& evt);
};