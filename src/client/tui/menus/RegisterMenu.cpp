#include "RegisterMenu.hpp"

using namespace ftxui;

void
registerMenu(ClientSession& clientSession)
{

    auto screen = ScreenInteractive::Fullscreen();

    auto titlebox = Renderer(
        [&]
        { return hbox({text(GAME_TITLE) | bold | color(Color::Green1)}); });

    // ------- USERNAME AND PASSWORD INPUTS -------- //

    std::string inputUsernameString;
    std::string inputPasswordString;
    std::string confirmPasswordString;

    Component inputUsername = Input(&inputUsernameString, "Type in your username");

    InputOption passwordOption;
    passwordOption.password = true;  // hide pwd
    Component inputPassword =
        Input(&inputPasswordString, "Type in your password", passwordOption);
    Component inputConfirmPassword =
        Input(&confirmPasswordString, "Confirm your password", passwordOption);

    std::string errorMessage;

    // -------- BUTTONS  ----------- //
    auto registerButton =
        Button("Register",
               [&screen, &inputUsernameString, &inputPasswordString, &confirmPasswordString, &clientSession, &errorMessage]
               {

                   if (inputUsernameString.empty() || inputPasswordString.empty() || confirmPasswordString.empty())
                   {
                       errorMessage = "Please fill in all fields";
                   }
                   else if (inputPasswordString != confirmPasswordString) {
                       errorMessage = "Passwords do not match";
                   }
                   else
                   {
                       StatusCode status = clientSession.registerPlayer(inputUsernameString, inputPasswordString);
                       if (status == StatusCode::SUCCESS)
                       {
                           currMenu = MenuState::loginMenu;
                           screen.Exit();
                       }
                       else if (status == StatusCode::ERROR_USERNAME_TAKEN)
                       {
                           errorMessage = "Username already taken";
                       }
                       else
                       {
                           errorMessage = "Registration failed. Please try again.";
                       }
                     }
               });

    auto loginButton =
        Button("Back to login",
               [&screen]
               {
                   currMenu = MenuState::loginMenu; // Si décide de pas créer de
                                                    // nouveau compte
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
        Container::Vertical({inputUsername, inputPassword, inputConfirmPassword,
                             registerButton, loginButton, quitterButton});

    // --- DIFFERENT BOXES --- //
    auto buttonBox = Renderer(
        components,
        [&]
        {
            return hbox(registerButton->Render() | color(Color::Green1),
                        loginButton->Render() | color(Color::Green1));
        });

    auto loginBox = Renderer(
        components,
        [&]
        {
            return hbox({vbox(
                {text(errorMessage) | color(Color::Red), // Message d'erreur
                 hbox(text("Username: ") | color(Color::Green1),
                      inputUsername->Render()),
                 hbox(text("Password: ") | color(Color::Green1),
                      inputPassword->Render()),
                 hbox(text("Confirm Password: ") | color(Color::Green1),
                      inputConfirmPassword->Render()),
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
