#pragma once

#include <unordered_map>
#include <string>
#include <json/json.h>
#include <utility>
#include <iostream>
#include <mutex>
#include <memory>

#include "../../../utilities/utility/utility.hpp"
#include "../writer/logWriter.hpp"

class LogInterpreter
{
public:
    struct LogObj
    {
        std::string id;
        std::string createdDateTime;
        time_t captureDate;

        LogObj(
            std::string _id,
            std::string _createdDateTime,
            time_t _captureDate) : id(_id),
                                   createdDateTime(_createdDateTime),
                                   captureDate(_captureDate) {}
    };

private:
    std::shared_ptr<LogWriter> logWriter = NULL;
    std::unordered_map<std::string, LogObj> logs;
    std::mutex logsLock;

public:
    LogInterpreter(std::shared_ptr<LogWriter> _logWriter) : logWriter(_logWriter) {}

    // { value : [values] }
    void interpretJsonStr(std::string jsonStr, const std::string &dateKey);

    void cleareCacheBefore(time_t cleareBefore);
};