#pragma once

#include <map>
#include <ftxui/component/event.hpp>
#include "tetris-common/types.hpp"

class inputHandler {
    std::map<Event, Action>  inputMap;
    Event currentEvent;

public:
    inputHandler() : currentEvent(Event::Custom) { setupController(); }

    void setupController() {
        // if (!tetrisGame.getIsInvCmd()) {
        //     inputMap[Event::ArrowLeft]      = MoveLeft;
        //     inputMap[Event::ArrowRight]     = MoveRight;
        // } else {
        //     inputMap[Event::ArrowRight]      = MoveLeft;
        //     inputMap[Event::ArrowLeft]     = MoveRight;
        // }
        
        inputMap[Event::ArrowLeft]      = MoveLeft;
        inputMap[Event::ArrowRight]     = MoveRight;

        inputMap[Event::ArrowDown]      = MoveDown;

        inputMap[Event::ArrowUp]        = RotateRight;
        inputMap[Event::Character('q')] = RotateRight;
        inputMap[Event::Character('e')] = RotateLeft;

        inputMap[Event::Character(' ')] = InstantFall;

        inputMap[Event::Character('b')] = UseBag;
    }

    Action getUserAction() {
        if (const auto it = inputMap.find(currentEvent); it != inputMap.end()) {
            const Action a = it->second;
            return a;
        }

        return None;
    }

    Action handleInputs() {
        const Action action = getUserAction();
        return action;
    }

    // Swap the actions for left and right
    void invertKeys() {
        const Action leftAction      = inputMap[Event::ArrowLeft];
        inputMap[Event::ArrowLeft]   = inputMap[Event::ArrowRight];
        inputMap[Event::ArrowRight]  = leftAction;
    }

    void setEvent(const Event& evt)  { currentEvent = evt; }
};