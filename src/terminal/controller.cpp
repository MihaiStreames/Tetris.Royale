#include "controller.hpp"

Controller::Controller() : coolDown(0) {}

void Controller::setupController() {
    inputMap[ftxui::Event::ArrowLeft]       = MoveLeft;
    inputMap[ftxui::Event::ArrowRight]      = MoveRight;
    inputMap[ftxui::Event::ArrowDown]       = MoveDown;
    inputMap[ftxui::Event::Character('q')]  = RotateRight;
    inputMap[ftxui::Event::Character('e')]  = RotateLeft;
    inputMap[ftxui::Event::Character('b')]  = UseBag;
}

Action Controller::getInput() const {
    if (const auto it = inputMap.find(currentEvent); it != inputMap.end()) {
        return it->second;
    }

    return None;
}

void Controller::invertedKeys() {
    const Action leftAction = inputMap[ftxui::Event::ArrowLeft];
    inputMap[ftxui::Event::ArrowLeft] = inputMap[ftxui::Event::ArrowRight];
    inputMap[ftxui::Event::ArrowRight] = leftAction;
}