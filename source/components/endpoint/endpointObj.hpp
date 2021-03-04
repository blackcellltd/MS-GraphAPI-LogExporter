#pragma once

#include <thread>

#include "../../providers/config/config.hpp"
#include "../log/interpreter/logInterpreter.hpp"
#include "../log/source/logSource.hpp"
#include "../log/writer/logWriter.hpp"
#include "../../providers/accessToken/accessToken.hpp"
#include "../timmer/timmer.hpp"

class EndpointObj : public Timmer::Timmerable
{
public:
    struct ThreadObj
    {
        std::thread thread;
        std::shared_ptr<bool> isFinished = std::make_shared<bool>(false);
    };

private:
    std::string name;
    std::shared_ptr<AccessToken> accessToken;
    std::shared_ptr<LogSource> logSource;
    std::shared_ptr<LogInterpreter> logInterpreter;
    std::shared_ptr<LogWriter> logWriter;

    std::list<std::shared_ptr<ThreadObj>> threads;

public:
    EndpointObj(
        const Config::EndpointConf &endpointConf,
        std::shared_ptr<AccessToken> accessToken);

    void signal();

protected:
    void removeFinishedThreads();
};