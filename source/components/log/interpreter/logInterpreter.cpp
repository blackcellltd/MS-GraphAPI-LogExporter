#include "logInterpreter.hpp"

#include "../../../utilities/runtimeError.hpp"
#include "../../../providers/config/config.hpp"

#include <sstream>

void LogInterpreter::interpretJsonStr(std::string jsonStr, const std::string &dateKey)
{
    Json::Value data;
    std::stringstream(jsonStr) >> data;

    const Json::Value &values = data["value"].isNull()
                                    ? throw RunntimeError("Log Interpreter: error in fetching fields (value)")
                                    : data["value"];
    this->logsLock.lock();
    this->logWriter->startBuffer();

    int statAllLogCount = values.size();
    int statNewLogCount = 0;
    for (const Json::Value &value : values)
    {
        std::string id = value["id"].isNull()
                             ? throw RunntimeError("Log Interpreter: error in fetching fields (id)")
                             : value["id"].asString();
        if (this->logs.find(id) == this->logs.end())
        {
            LogObj tmpObj(
                id,
                value[dateKey].isNull()
                    ? throw RunntimeError("Log Interpreter: error in fetching fields (createdDateTime)")
                    : value[dateKey].asString(),
                timeUTC());

            this->logs.insert(std::make_pair(id, tmpObj));
            this->logWriter->addBuffer(value);
            statNewLogCount++;
        }
    }
    this->logWriter->writeBuffer();
    this->logsLock.unlock();

    logger.information(
        "Log Interpreter: processed log entries: " +
        std::to_string(statAllLogCount) +
        ", new: " +
        std::to_string(statNewLogCount) +
        ", cached: " +
        std::to_string(this->logs.size()));
}

void LogInterpreter::cleareCacheBefore(time_t cleareBefore)
{
    int count = 0;

    this->logsLock.lock();

    for (
        std::unordered_map<std::string, LogInterpreter::LogObj>::iterator iter = this->logs.begin();
        iter != this->logs.end();)
        if (iter->second.captureDate < cleareBefore)
        {
            count++;
            iter = this->logs.erase(iter);
        }
        else
            iter++;

    this->logsLock.unlock();
    logger.debug("Clear Cache: " + std::to_string(count) + " was deleted");
}