#include "LoginMenu.hpp"

using namespace ftxui;

void
loginMenu(ClientSession& clientSession)
{

    auto screen = ScreenInteractive::Fullscreen();

    // Titre
    auto titlebox = Renderer(
        [&]
        { return hbox({text(GAME_TITLE) | bold | color(Color::Green1)}); });


    std::string inputUsernameString;
    std::string inputPasswordString;

    // ------- USERNAME AND PASSWORD INPUTS -------- //
    Component inputUsername = Input(&inputUsernameString, "Type in your username");

    InputOption passwordOption;
    passwordOption.password = true;  // hide the password

    Component inputPassword =
        Input(&inputPasswordString, "Type in your password", passwordOption);

    std::string errorMessage;

    // -------- BUTTONS  ----------- //
    auto loginButton =
        Button("Login",
               [&screen, &clientSession, &inputUsernameString, &inputPasswordString, &errorMessage]
               {

                    if (inputUsernameString.empty() || inputPasswordString.empty()) {
                        errorMessage = "Please fill in both fields";
                    // check if login worked
                    } else if (clientSession.loginPlayer(inputUsernameString, inputPasswordString) == StatusCode::SUCCESS) {
                        currMenu = MenuState::mainMenu;
                        screen.Exit();
                    } else {
                        errorMessage = "Login failed. Please try again.";
                    }

               });

    auto registerButton = Button("Register",
                                 [&screen]
                                 {
                                     currMenu = MenuState::registerMenu;
                                     screen.Exit();
                                 });

    auto quitterButton = Button("Quit",
                                [&screen]
                                {
                                    currMenu = MenuState::quitter;
                                    screen.Exit();
                                });

    // ----- INTERACTION COMPONENTS  -----  //
    auto components =
        Container::Vertical({inputUsername, inputPassword, loginButton,
                             registerButton, quitterButton});

    // --- DIFFERENT BOXES --- //
    auto buttonBox = Renderer(
        components,
        [&]
        {
            return hbox(loginButton->Render() | color(Color::Green1),
                        registerButton->Render() | color(Color::Green1));
        });

    auto loginBox = Renderer(
        components,
        [&]
        {
            return hbox({vbox({text(errorMessage) | color(Color::Red),
                               hbox(text("Username: ") | color(Color::Green1),
                                    inputUsername->Render()),
                               hbox(text("Password: ") | color(Color::Green1),
                                    inputPassword->Render()),
                               buttonBox->Render()})});
        });

    auto halfbottomBox = Renderer(
        components,
        [&]
        {
            return hbox(
                {loginBox->Render(), filler(),
                 vbox({filler(), hbox({filler(), quitterButton->Render() |
                                                     color(Color::Green1)})})});
        });

    auto mainBox =
        Renderer(halfbottomBox,
                 [&]
                 {
                     return vbox({
                         filler(),
                         window(text("") | color(Color::Green1),
                                vbox({titlebox->Render(),
                                      separator() | color(Color::Green1),
                                      halfbottomBox->Render(), filler()})) |
                             color(Color::Green1),
                         filler(),
                     });
                 });

    screen.Loop(mainBox);
}
