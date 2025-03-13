#pragma once

#include <memory>
#include <string>
// FTXUI includes
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

void registerMenu(ClientSession& clientSession);