#include "config.hpp"

#include <fstream>
#include <iostream>
#include <json/json.h>

std::shared_ptr<const Validation::Rule> json_notNull = std::make_shared<const Validation::Rule>(
    [](const void *arg) -> bool {
        const Json::Value *value = static_cast<const Json::Value *>(arg);

        return !value->isNull();
    },
    "is required");

std::shared_ptr<const Validation::Rule> json_isTypeString = std::make_shared<const Validation::Rule>(
    [](const void *arg) -> bool {
        const Json::Value *value = static_cast<const Json::Value *>(arg);

        return value->isString();
    },
    "is must be a string");

std::shared_ptr<const Validation::Rule> json_isTypeInteger = std::make_shared<const Validation::Rule>(
    [](const void *arg) -> bool {
        const Json::Value *value = static_cast<const Json::Value *>(arg);

        return value->isInt();
    },
    "is must be an integer");

std::shared_ptr<const Validation::Rule> json_isTypeArray = std::make_shared<const Validation::Rule>(
    [](const void *arg) -> bool {
        const Json::Value *value = static_cast<const Json::Value *>(arg);

        return value->isArray();
    },
    "is must be an array");

std::shared_ptr<const Validation::Rule> json_isStringNotEmpty = std::make_shared<const Validation::Rule>(
    [](const void *arg) -> bool {
        const Json::Value *value = static_cast<const Json::Value *>(arg);

        return value->asString().length() > 0;
    },
    "is must not be empty");

std::vector<Config::keyRuleChain> Config::rules{
    Config::keyRuleChain{
        "Access Intervals",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeInteger)},

    Config::keyRuleChain{
        "Access Duration",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeInteger)},

    Config::keyRuleChain{
        "Access Delay",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeInteger)},

    Config::keyRuleChain{
        "History Access Duration",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeInteger)},

    Config::keyRuleChain{
        "History Duration",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeInteger)},

    Config::keyRuleChain{
        "Log Path",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)},

    Config::keyRuleChain{
        "Log Level",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeInteger)},

    Config::keyRuleChain{
        "Client Id",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)},

    Config::keyRuleChain{
        "Client Secret",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)},

    Config::keyRuleChain{
        "Tenant Id",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)},

    Config::keyRuleChain{
        "Api Base Url",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)},

    Config::keyRuleChain{
        "Endpoints",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeArray)}};

std::vector<Config::keyRuleChain> Config::endpointRules{
    Config::keyRuleChain{
        "Name",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)},

    Config::keyRuleChain{
        "Endpoint path",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)},

    Config::keyRuleChain{
        "Timefield name",
        Validation::RuleChain()
            .add(json_notNull)
            .add(json_isTypeString)
            .add(json_isStringNotEmpty)}};

std::string Config::getString(std::string key) const
{
    return getJson(key).asString();
}

int Config::getInt(std::string key) const
{
    return getJson(key).asInt();
}

bool Config::getBool(std::string key) const
{
    return getJson(key).asBool();
}

const Json::Value &Config::getJson(std::string key) const
{
    if (isEmpty())
    {
        logger.critical("Config: data was not loaded yet");
        throw std::runtime_error("dat was not loaded yet");
    }

    const Json::Value &value = this->data[key];

    if (value.type() == Json::nullValue)
        throw RunntimeError("Config: key not found (" + key + ")");

    return value;
}

void Config::loadFromFile()
{
    const std::string &filePath = this->configFilePath;
    logger.information("Config: starting to load configuration from " + filePath);
    std::ifstream fileStream(filePath);
    if (!(fileStream.is_open()))
        throw RunntimeError("Config validation: error opening the given file (" + filePath + ")");

    Json::Value data;
    fileStream >> data;
    fileStream.close();

    Config::validateData(data);
    logger.success("Config: configuration loaded from " + filePath);
    this->data = data;

    this->enpointsData.clear();
    const Json::Value &enpointsData = data["Endpoints"];

    for (Json::Value::ArrayIndex i = 0; i < enpointsData.size(); i++)
    {
        const Json::Value &endpoitData = enpointsData[i];

        this->enpointsData.push_back(Config::EndpointConf(
            endpoitData["Name"].asString(),
            endpoitData["Endpoint path"].asString(),
            endpoitData["Timefield name"].asString()));
    }

    this->isempty = false;
}

void Config::validateData(const Json::Value &value)
{
    for (keyRuleChain &rule : Config::rules)
        if (!(rule.chain.validate(&(value[rule.key]))))
            throw RunntimeError("Config validation: " + rule.key +
                                " " + rule.chain.getErrorText());

    const Json::Value &enpoints = value["Endpoints"];

    for (int i = 0; i < (int)enpoints.size(); i++)
        for (keyRuleChain &endpointRule : Config::endpointRules)
            if (!(endpointRule.chain.validate(&(enpoints[i][endpointRule.key]))))
                throw RunntimeError(
                    "Config validation: in endpont [" +
                    std::to_string(i + 1) +
                    "]: " +
                    endpointRule.key +
                    " " +
                    endpointRule.chain.getErrorText());
}

void Config::validateFile(const std::string &filePath)
{
    std::ifstream fileStream(filePath);
    if (!(fileStream.is_open()))
        throw RunntimeError(
            "Config validation: error opening the given file (" +
            filePath + ")");

    Json::Value data;
    fileStream >> data;
    fileStream.close();

    Config::validateData(data);
}
