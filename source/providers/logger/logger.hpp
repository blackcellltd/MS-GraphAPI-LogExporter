#pragma once

#include <ostream>
#include <string>
#include <mutex>

class Logger;

extern Logger logger;

class Logger
{
public:
    enum Level
    {
        Critical = 0,
        Error = 1,
        Success = 2,
        Information = 3,
        Debug = 4,
        Request = 5,
    };

private:
    struct LevelMetadData
    {
        const std::string prefix;

        LevelMetadData(const std::string _prefix) : prefix(_prefix) {}
    };
    const LevelMetadData levelMetaData[6] = {
        LevelMetadData("[!]"), // Critical
        LevelMetadData("[-]"), // Error
        LevelMetadData("[+]"), // Success
        LevelMetadData("[*]"), // Information
        LevelMetadData("[D]"), // Debug
        LevelMetadData("[R]"), // Request
    };

    std::ostream *stream;
    Level loglevel;
    std::mutex logLock;

public:
    Logger(
        std::ostream *_stream,
        Level _loglevel = Level::Information) : stream(_stream),
                                                loglevel(_loglevel) {}

    void log(Level level, const std::string &text);

    void setLogLevel(Level level)
    {
        this->logLock.lock();

        this->loglevel = level;

        this->logLock.unlock();
    }

    bool isLogLevel(Level level) const
    {
        return level <= this->loglevel;
    }

    Level getLogLevel() const
    {
        return this->loglevel;
    }

    inline void request(const std::string &text)
    {
        log(Level::Request, text);
    }

    inline void critical(const std::string &text)
    {
        log(Level::Critical, text);
    }

    inline void error(const std::string &text)
    {
        log(Level::Error, text);
    }

    inline void success(const std::string &text)
    {
        log(Level::Success, text);
    }

    inline void information(const std::string &text)
    {
        log(Level::Information, text);
    }

    inline void debug(const std::string &text)
    {
        log(Level::Debug, text);
    }
};