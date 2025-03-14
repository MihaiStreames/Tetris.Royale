#include "LoginScreen.hpp"
#include "Common.hpp"

using namespace ftxui;

void showLoginScreen(ClientSession &session) {
    auto screen = ScreenInteractive::Fullscreen();

    // Input fields
    std::string username;
    std::string password;
    std::string errorMessage;

    InputOption passwordOption;
    passwordOption.password = true; // hide the password

    // Create input components
    auto usernameInput = Input(&username, "Username");
    auto passwordInput = Input(&password, "Password", passwordOption);

    // Create buttons
    auto loginButton = Button("Login", [&] {
        if (username.empty() || password.empty()) {
            errorMessage = "Please enter both username and password";
            return;
        }

        // Try to login
        std::cerr << "DEBUG: Attempting to login with username: " << username << std::endl;
        StatusCode result = session.loginPlayer(username, password);
        std::cerr << "DEBUG: loginPlayer returned: " << getStatusCodeString(result) << std::endl;

        if (result == StatusCode::SUCCESS) {
            // Start session after successful login
            std::cerr << "DEBUG: Login successful, starting session" << std::endl;
            StatusCode sessionResult = session.startSession();
            std::cerr << "DEBUG: startSession returned: " << getStatusCodeString(sessionResult) << std::endl;
            std::cerr << "DEBUG: Token after startSession: '" << session.getToken() << "'" << std::endl;

            if (sessionResult == StatusCode::SUCCESS) {
                currentScreen = ScreenState::MainMenu;
                screen.Exit();
            } else {
                errorMessage = "Failed to start session: " + getStatusCodeString(sessionResult);
            }
        } else {
            errorMessage = "Login failed. Please check your credentials.";
        }
    });

    auto registerButton = Button("Register", [&] {
        currentScreen = ScreenState::Register;
        screen.Exit();
    });

    auto exitButton = Button("Exit", [&] {
        currentScreen = ScreenState::Exit;
        screen.Exit();
    });

    // Container for all components
    auto container = Container::Vertical({
        usernameInput,
        passwordInput,
        loginButton,
        registerButton,
        exitButton
    });

    // Renderer for the layout
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

    // Main loop
    screen.Loop(renderer);
}
