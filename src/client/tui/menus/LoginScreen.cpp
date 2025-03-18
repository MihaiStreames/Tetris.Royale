#include "LoginScreen.hpp"

using namespace ftxui;

void showLoginScreen(ClientSession &session) {

    // this function will display the login screen
    // it will ask the user for their username and password
    // and then try to login with the provided credentials

    auto screen = ScreenInteractive::Fullscreen();
    currentScreen = ScreenState::Exit;

    // input fields
    std::string username;
    std::string password;

    // error message -> will be displayed if anything goes wrong
    std::string errorMessage;

    InputOption passwordOption;
    passwordOption.password = true; // hide the password (weird workaround)

    // input components
    auto usernameInput = Input(&username, "Username");
    auto passwordInput = Input(&password, "Password", passwordOption);

    // we will create a button for each action

    auto loginButton = Button("Login", [&] {

        // we want to check if the user has entered both username and password
        if (username.empty() || password.empty()) {
            errorMessage = "Please enter both username and password";
            return;
        }

        // try to login and fetch the status code
        StatusCode result = session.loginPlayer(username, password);

        if (result == StatusCode::SUCCESS) {

            StatusCode sessionResult = session.startSession();

            // if login was successful, we will try to start a new session
            if (sessionResult == StatusCode::SUCCESS) {

                currentScreen = ScreenState::MainMenu;
                screen.Exit();
            
            // if starting a session failed, we will display an error message
            } else {
                errorMessage = "Failed to start session: " + getStatusCodeString(sessionResult);
            }

        // if login failed, we will display an error message
        } else {
            errorMessage = "Login failed. Please check your credentials.";
        }
    });

    auto registerButton = Button("Register", [&] {
        // if the user wants to register, we will switch to the register screen
        currentScreen = ScreenState::Register;
        screen.Exit();
    });

    auto exitButton = Button("Exit", [&] {
        // if the user wants to exit, we will switch to the exit screen
        currentScreen = ScreenState::Exit;
        screen.Exit();
    });

    // define a container for all components
    auto container = Container::Vertical({
        usernameInput,
        passwordInput,
        loginButton,
        registerButton,
        exitButton
    });

    // and then finally render the whole layout (this is usually where the code becomes
    // disgusting and unreadable, please forgive us for having one big function)
    auto renderer = Renderer(container, [&] {
        return vbox({
                   text("TETRIS ROYALE") | bold | center,
                   separator(),
                   text(errorMessage) | color(Color::Red),
                   hbox(text("Username: "), usernameInput->Render()),
                   hbox(text("Password: "), passwordInput->Render()),
                   separator(),
                   hbox({
                       loginButton->Render() | center,
                       registerButton->Render() | center,
                       exitButton->Render() | center
                   })
               }) | border | color(Color::Green);
    });

    // main loop thingy (ftxui)
    screen.Loop(renderer);

}


