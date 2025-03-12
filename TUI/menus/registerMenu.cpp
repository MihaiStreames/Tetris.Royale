#include "registerMenu.hpp"


using namespace ftxui;

void registerMenu() {
    auto& data = testData;
    auto screen = ScreenInteractive::Fullscreen();

    auto titlebox = Renderer([&] {
        return hbox({
            text(data.gameTitle) | bold | color(Color::Green1)
        });
    });

    // ------- USERNAME AND PASSWORD INPUTS -------- // 
    Component inputUsername = Input(&data.username, "Type in your username");

    InputOption passwordOption;
    passwordOption.password = true;
    Component inputPassword = Input(&data.password, "Type in your password", passwordOption);
    Component inputConfirmPassword = Input(&data.passwordConfirm, "Confirm your password", passwordOption);

    // -------- BUTTONS  ----------- //
    auto registerButton = Button("Register", [&screen] {
        // ADD LES INFOS DANS LA DB 
        // if (data.password == data.passwordConfirm){ 
        //    currMenu = MenuState::loginMenu; // Login car doit se connecter avec nouveau compte
        //    screen.Exit();
        // }
    });

    auto loginButton = Button("Back to login", [&screen] {
        currMenu = MenuState::loginMenu; // Si décide de pas créer de nouveau compte
        screen.Exit();
    });

    auto quitterButton = Button("Quitter", [&screen] {
        currMenu = MenuState::quitter;
        screen.Exit();
    });

    // ----- INTERACTION COMPONENTS  -----  //
    auto components = Container::Vertical({
        inputUsername,
        inputPassword,
        inputConfirmPassword,
        registerButton,
        loginButton,
        quitterButton
    });

    // --- DIFFERENT BOXES --- //
    auto buttonBox = Renderer(components, [&] {
        return hbox(
            registerButton->Render() | color(Color::Green1),
            loginButton->Render() | color(Color::Green1)
        );
    });

    auto loginBox = Renderer(components, [&] {
        return hbox({
            vbox({
                hbox(text("Username: ") | color(Color::Green1), inputUsername->Render()),
                hbox(text("Password: ") | color(Color::Green1), inputPassword->Render()),
                hbox(text("Confirm Password: ") | color(Color::Green1),
                            inputConfirmPassword->Render()),
                buttonBox->Render()
            })
        });
    });

    auto halfbottomBox = Renderer(components, [&] {
        return hbox({
            loginBox->Render(),
            filler(),
            vbox({
                filler(),
                hbox({
                    filler(),
                    quitterButton->Render() | color(Color::Green1)
                })
            })
        });
    });

    auto mainBox = Renderer(halfbottomBox, [&] {
        return vbox({
            filler(),
            window(
                text("") | color(Color::Green1),
                vbox({
                    titlebox->Render(),
                    separator() | color(Color::Green1),
                    halfbottomBox->Render(),
                    filler()
                })
            ) | color(Color::Green1),
            filler(),
        });
    });

    screen.Loop(mainBox);
}
