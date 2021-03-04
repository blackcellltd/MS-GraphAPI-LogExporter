#pragma once

#include <string>
#include <list>
#include <memory>
#include <iostream>

#include "../../utilities/runtimeError.hpp"

namespace Validation
{

    class Rule
    {
    private:
        bool (*validate)(const void *arg);
        const std::string errorText;

    public:
        Rule(bool (*_validate)(const void *arg), const std::string _errorText) : validate(_validate), errorText(_errorText) {}

        bool check(const void *arg) const
        {
            return (*(this->validate))(arg);
        }

        const std::string &getErrorText() const
        {
            return this->errorText;
        }
    };

    class RuleChain
    {
    private:
        std::list<std::shared_ptr<const Validation::Rule>> rules;
        std::string lastError = "";

    public:
        RuleChain &add(std::shared_ptr<const Validation::Rule> &rule)
        {
            rules.push_back(rule);

            return *this;
        }

        bool validate(const void *arg)
        {
            this->lastError = "";
            for (std::shared_ptr<const Validation::Rule> rule : rules)
                if (!rule->check(arg))
                {
                    this->lastError = rule->getErrorText();
                    return false;
                }

            return true;
        }

        const std::string &getErrorText()
        {
            return this->lastError;
        }
    };

} // namespace Validation