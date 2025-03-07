#include "inputHandler.hpp"

using namespace ftxui;

void inputHandler::setupController() {
    inputMap[Event::ArrowLeft]      = Action::MoveLeft;
    inputMap[Event::ArrowRight]     = Action::MoveRight;

    inputMap[Event::ArrowDown]      = Action::MoveDown;

    inputMap[Event::ArrowUp]        = Action::RotateRight;
    inputMap[Event::Character('q')] = Action::RotateRight;
    inputMap[Event::Character('e')] = Action::RotateLeft;

    inputMap[Event::Character(' ')] = Action::InstantFall;

    inputMap[Event::Character('b')] = Action::UseBag;

    inputMap[Event::Character('m')] = Action::UseMalus;
    inputMap[Event::Character('l')] = Action::UseBonus;
}

Action inputHandler::getUserAction() {
    if (const auto it = inputMap.find(currentEvent); it != inputMap.end()) {
        const Action a = it->second;
        return a;
    }

    return Action::None;
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