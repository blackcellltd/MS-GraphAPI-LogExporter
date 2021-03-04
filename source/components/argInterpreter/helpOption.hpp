#pragma once

#include <iostream>

void printHelp()
{
    std::cout
        << "Options: -h, --help                              Show basic help message" << std::endl
        << "         -vc, --validate-config [Config file]    Validate a given config file" << std::endl
        << "         -scf, --set-configFile <Config file>    Set config file" << std::endl
        << std::endl
        << std::endl
        << "For more indept help please refer to the official GitHub page" << std::endl;
}