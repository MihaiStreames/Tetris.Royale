#include "settingsMenu.hpp"

using namespace ftxui;

void settingsMenu() {
    auto &data = testData;
    auto screen = ScreenInteractive::Fullscreen();

    auto titlebox = Renderer([&] {
        return hbox({
            text(data.gameTitle)
            | bold
            | color(Color::Green1)
        });
    });

    auto back_button = Button("Back to Main Menu", [&screen] {
        currMenu = MenuState::mainMenu;
        screen.Exit();
    });

    auto component = Container::Vertical({
        back_button
    });

    // We'll generate a list from data.keyBindings
    std::vector<std::string> orderedKeys = {"Left", "Right", "Rotate", "Drop", "Switch Player", "Power-up"};


    // TROUVER COMMENT CHANGER DE TOUCHES AVEC UN INPUT 
    std::vector<Element> commandList;
    for (auto &move: orderedKeys) {
        commandList.push_back(
            border(hbox({
                text(move) | color(Color::Green1),
                separator(),
                text("(" + data.keyBindings[move] + ")") | color(Color::Green1)
            }))
        );
    }

    auto buttonBox = Renderer(component, [&] {
        return hbox(back_button->Render());
    });

    auto halfbottomBox = Renderer(component, [&] {
        return hbox({
            filler(),
            vbox({
                filler(),
                hbox({
                    filler(),
                    buttonBox->Render()
                })
            })
        });
    });

    auto mainBox = Renderer(halfbottomBox, [&] {
        return vbox({
                   titlebox->Render(),
                   vbox(commandList) | color(Color::Green1),
                   separator() | color(Color::Green1),
                   hbox({filler(), halfbottomBox->Render()})
               }) | border | color(Color::Green1);
    });

    screen.Loop(mainBox);
}
