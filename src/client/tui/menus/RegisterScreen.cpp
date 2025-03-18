#include "RegisterScreen.hpp"

using namespace ftxui;


void showRegisterScreen(ClientSession &session) {

    // this is the register screen
    // it will be displayed when the user clicks the register button on the login screen
    // it will allow the user to register a new account
    // the user will be able to enter a username and password
    // the user will also need to confirm the password
    // the user will be able to click a register button to register the account
    // the user will also be able to click a back button to go back to the login screen

    auto screen = ScreenInteractive::Fullscreen();
    currentScreen = ScreenState::Exit;

    // input fields
    std::string username;
    std::string password;
    std::string confirmPassword;

    std::string errorMessage;

    InputOption passwordOption;
    passwordOption.password = true; // hide the password

    // input components
    auto usernameInput = Input(&username, "Username");
    auto passwordInput = Input(&password, "Password", passwordOption);
    auto confirmInput = Input(&confirmPassword, "Confirm Password", passwordOption);

    // creating all the buttons
    auto registerButton = Button("Register", [&] {
        
        // if any of the fields are empty, show an error message
        if (username.empty() || password.empty() || confirmPassword.empty()) {
            errorMessage = "All fields are required";
            return;
        }

        // if the passwords do not match, show an error message
        if (password != confirmPassword) {
            errorMessage = "Passwords do not match";
            return;
        }

        // try to register the player
        StatusCode result = session.registerPlayer(username, password);

        // if it was successful, go back to the login screen
        if (result == StatusCode::SUCCESS) {

            currentScreen = ScreenState::Login;
            screen.Exit();
        
        // if the username is already taken, show an error message
        } else if (result == StatusCode::ERROR_USERNAME_TAKEN) {

            errorMessage = "Username already taken";

        // if there was an error, show a generic error message
        } else {
            errorMessage = "Registration failed. Please try again.";
        }
    });

    auto backButton = Button("Back to Login", [&] {
        // go back to the login screen
        currentScreen = ScreenState::Login;
        screen.Exit();
    });

    // container for all the components
    auto container = Container::Vertical({
        usernameInput,
        passwordInput,
        confirmInput,
        registerButton,
        backButton
    });

    // renderer
    auto renderer = Renderer(container, [&] {
        return vbox({
                   text("REGISTER ACCOUNT") | bold | center,
                   separator(),
                   text(errorMessage) | color(Color::Red),
                   hbox(text("Username: "), usernameInput->Render()),
                   hbox(text("Password: "), passwordInput->Render()),
                   hbox(text("Confirm: "), confirmInput->Render()),
                   separator(),
                   hbox({
                       registerButton->Render() | center,
                       backButton->Render() | center
                   })
               }) | border | color(Color::Green);
    });

    // main loop thingy
    screen.Loop(renderer);

}

