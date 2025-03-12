#include "loginMenu.hpp"

using namespace ftxui;

void loginMenu() {
    auto& data = testData;
    auto screen = ScreenInteractive::Fullscreen();

    // Titre
    auto titlebox = Renderer([&] {
        return hbox({
            text(data.gameTitle) | bold | color(Color::Green1)
        });
    });

    // ------- USERNAME AND PASSWORD INPUTS -------- // 
    Component inputUsername = Input(&data.username, "Type in your username");

    // Input pour le mot de passe
    InputOption passwordOption;
    passwordOption.password = true;
    Component inputPassword = Input(&data.password, "Type in your password", passwordOption);

    // -------- BUTTONS  ----------- //
    auto loginButton = Button("Login", [&screen] {
        //Vérification des données dans la DB ici
        // if (data.username in db && data.password == username.password){
        //  currMenu = MenuState::mainMenu;
        //  screen.Exit();
        //} else { cout << "Wrong username or password" << endl; }
    });

    auto registerButton = Button("Register", [&screen] {
        currMenu = MenuState::registerMenu;
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
        loginButton,
        registerButton,
        quitterButton
    });

    // --- DIFFERENT BOXES --- //
    auto buttonBox = Renderer(components, [&] {
        return hbox(
            loginButton->Render() | color(Color::Green1),
            registerButton->Render() | color(Color::Green1)
        );
    });

    auto loginBox = Renderer(components, [&] {
        return hbox({
            vbox({
                hbox(text("Username: ") | color(Color::Green1), inputUsername->Render()),
                hbox(text("Password: ") | color(Color::Green1), inputPassword->Render()),
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
