#include <algorithm> // pour std::min
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

// FTXUI
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/util/ref.hpp"

#include "MenuHandler.hpp"

#include "Common.hpp"
#include "ClientSession.hpp"

void inLobbyMenu(ClientSession &clientSession);