#include "RegisterScreen.hpp"
#include "Common.hpp"

using namespace ftxui;

void showRegisterScreen(ClientSession& session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Input fields
    std::string username;
    std::string password;
    std::string confirmPassword;
    std::string errorMessage;

    InputOption passwordOption;
    passwordOption.password = true;  // hide the password

    // Create input components
    auto usernameInput = Input(&username, "Username");
    auto passwordInput = Input(&password, "Password", passwordOption);
    auto confirmInput = Input(&confirmPassword, "Confirm Password", passwordOption);

    // Create buttons
    auto registerButton = Button("Register", [&] {
        if (username.empty() || password.empty() || confirmPassword.empty()) {
            errorMessage = "All fields are required";
            return;
        }

        if (password != confirmPassword) {
            errorMessage = "Passwords do not match";
            return;
        }

        // Try to register
        StatusCode result = session.registerPlayer(username, password);
        if (result == StatusCode::SUCCESS) {
            currentScreen = ScreenState::Login;
            screen.Exit();
        } else if (result == StatusCode::ERROR_USERNAME_TAKEN) {
            errorMessage = "Username already taken";
        } else {
            errorMessage = "Registration failed. Please try again.";
        }
    });

    auto backButton = Button("Back to Login", [&] {
        currentScreen = ScreenState::Login;
        screen.Exit();
    });

    // Container for all components
    auto container = Container::Vertical({
        usernameInput,
        passwordInput,
        confirmInput,
        registerButton,
        backButton
    });

    // Renderer for the layout
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

    // Main loop
    screen.Loop(renderer);
}