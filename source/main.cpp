// Author: Laszlo Molnar @ Black Cell

#include <iostream>
#include <string>
#include <unistd.h>

#include "providers/config/config.hpp"
#include "providers/logger/logger.hpp"
#include "components/argInterpreter/argInterpreter.hpp"
#include "components/endpoint/endpointController.hpp"

Logger logger(&std::cout, static_cast<Logger::Level>(4));
Config config;

int main(int argc, char *argv[])
{
    ArgInterpreter::interpret(argc - 1, argv + 1);

    if (!config.getExecuteFlag())
        return 0;

    config.loadFromFile();
    logger.setLogLevel(static_cast<Logger::Level>(config.getInt("Log Level")));

    EndpointController::launch();

    while (true)
        pause();

    return 0;
}