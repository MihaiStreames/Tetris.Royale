#pragma once

#include <csignal>
#include <iostream>
#include <memory>


#include "Config.hpp"
#include "ClientSession.hpp"
#include "MenuHandler.hpp"

// Signal handler for SIGINT
void signalHandler(int signum);

// This is the Client entry point
int main(int argc, char *argv[]);

