#pragma once

#include <exception>
#include <string>

#include "../providers/logger/logger.hpp"

class RunntimeError : std::exception
{
private:
    std::string title;
    std::string body;

public:
    RunntimeError(const std::string &_title, const std::string &_body = "") : title(_title), body(_body)
    {
        logger.critical(this->title);
        if (this->body.length() != 0)
            logger.critical(this->body);
    }

    const char *what()
    {
        return this->title.c_str();
    }
};