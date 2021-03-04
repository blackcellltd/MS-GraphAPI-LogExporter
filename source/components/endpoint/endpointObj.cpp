#include "endpointObj.hpp"
#include "endpointAble.hpp"
#include "../../providers/logger/logger.hpp"
#include <chrono>
#include <thread>

EndpointObj::EndpointObj(
    const Config::EndpointConf &endpointConf,
    std::shared_ptr<AccessToken> accessToken)
{
    this->name = endpointConf.name;
    this->accessToken = accessToken;

    const std::string logPath =
        config.getString("Log Path") + "/" + endpointConf.name;
    this->logWriter =
        std::make_shared<LogWriter>(logPath);

    this->logInterpreter =
        std::make_shared<LogInterpreter>(this->logWriter);

    LogSource::Parameters logSourceParams{
        endpointConf.name,
        endpointConf.endpointPath,
        endpointConf.timefieldName};

    this->logSource =
        std::make_shared<LogSource>(
            this->accessToken,
            this->logInterpreter,
            logSourceParams);
}

static void taskThread(std::shared_ptr<bool> isFinised, std::shared_ptr<EndpointAble> taskAbleObj)
{
    auto statStart = std::chrono::high_resolution_clock::now();

    taskAbleObj->run();
    *isFinised = true;

    auto statEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> statElapsed = statEnd - statStart;
    logger.debug("Task: task took: " + std::to_string(statElapsed.count()) + "s");
}

void EndpointObj::signal()
{
    removeFinishedThreads();

    if (!this->logSource->canRun())
        return;

    std::shared_ptr<ThreadObj> threadPtr = std::make_shared<ThreadObj>();
    threadPtr->thread =
        std::move(
            std::thread(
                taskThread,
                threadPtr->isFinished,
                std::static_pointer_cast<EndpointAble>(this->logSource)));

    this->threads.push_front(threadPtr);
}

void EndpointObj::removeFinishedThreads()
{
    this->threads.remove_if(
        [](const std::shared_ptr<EndpointObj::ThreadObj> &value) -> bool {
            if (*(value->isFinished))
                value->thread.join();

            return *(value->isFinished);
        });
}