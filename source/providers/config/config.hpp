#pragma once

#include <vector>
#include <string>
#include <json/json.h>
#include <iostream>

#include "validator.hpp"
#include "../../utilities/runtimeError.hpp"

#define CONFIG_DEFAULT_FILE ("config.json")

class Config;

extern Config config;

class Config
{
public:
    struct EndpointConf
    {
        const std::string name;
        const std::string endpointPath;
        const std::string timefieldName;

        EndpointConf(
            const std::string &_name,
            const std::string &_endpointPath,
            const std::string &_timefieldName)
            : name(_name),
              endpointPath(_endpointPath),
              timefieldName(_timefieldName) {}
    };
    struct keyRuleChain
    {
        const std::string key;
        Validation::RuleChain chain;

        keyRuleChain(const std::string &_key, Validation::RuleChain _chain) : key(_key), chain(_chain) {}
    };

private:
    static std::vector<keyRuleChain> rules;
    static std::vector<keyRuleChain> endpointRules;

    bool isempty = true;
    std::string configFilePath = CONFIG_DEFAULT_FILE;
    Json::Value data;
    std::vector<EndpointConf> enpointsData;
    bool executeFlag = true;

public:
    std::string getString(std::string key) const;
    int getInt(std::string key) const;
    bool getBool(std::string key) const;
    const Json::Value &getJson(std::string key) const;
    bool isEmpty() const { return this->isempty; }

    void setConfigFilePath(const std::string &filePath)
    {
        this->configFilePath = filePath;
    }
    void loadFromFile();

    const std::vector<EndpointConf> &getEndpointConfigurations() const
    {
        return this->enpointsData;
    }

    static void validateData(const Json::Value &value);
    static void validateFile(const std::string &filePath);

    void unsetExecuteFlag() { this->executeFlag = false; };
    bool getExecuteFlag() { return this->executeFlag; }
};
