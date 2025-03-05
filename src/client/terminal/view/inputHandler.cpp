#include "inputHandler.hpp"

using namespace ftxui;

void inputHandler::setupController() {
    inputMap[Event::ArrowLeft]      = MoveLeft;
    inputMap[Event::ArrowRight]     = MoveRight;

    inputMap[Event::ArrowDown]      = MoveDown;

    inputMap[Event::ArrowUp]        = RotateRight;
    inputMap[Event::Character('q')] = RotateRight;
    inputMap[Event::Character('e')] = RotateLeft;

    inputMap[Event::Character(' ')] = InstantFall;

    inputMap[Event::Character('b')] = UseBag;

    inputMap[Event::Character('m')] = Malus;
    inputMap[Event::Character('l')] = Bonus;
}

Action inputHandler::getUserAction() {
    if (const auto it = inputMap.find(currentEvent); it != inputMap.end()) {
        const Action a = it->second;
        return a;
    }

    return None;
}

Action inputHandler::handleInputs() {
    const Action action = getUserAction();
    return action;
}

void inputHandler::invertKeys() {
    const Action leftAction      = inputMap[Event::ArrowLeft];
    inputMap[Event::ArrowLeft]   = inputMap[Event::ArrowRight];
    inputMap[Event::ArrowRight]  = leftAction;
}

void inputHandler::setEvent(const Event& evt)  { currentEvent = evt; }