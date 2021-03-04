#include "logSource.hpp"

#include "../../../providers/logger/logger.hpp"
#include "../../../providers/config/config.hpp"

const Request::Result LogSource::getLogs_betweenTimes(time_t from, time_t to)
{
    std::string token = this->token->getToken();
    if (token == "")
        return Request::Result(this->accessInterval, "");

    std::string timeTo = timestampToDate(addDelay(to));
    std::string timeFrom = timestampToDate(addDelay(from));
    std::string url =
        config.getString("Api Base Url") +
        this->params.apiEndpoint +
        "?$filter=(" +
        this->params.timeField + "%20ge%20" + timeFrom +
        ")%20and%20(" +
        this->params.timeField + "%20le%20" + timeTo + ")";

    logger.information(
        this->params.name + ": requesting logs from: " + timeFrom + ", to: " + timeTo);

    return Request()
        .setMethodGet()
        .setUrl(url)
        .addHeadderFild("Authorization: Bearer " + token)
        .exec();
}

void LogSource::routeRun()
{
    if (this->isdHistoryAccessEnabled &&
        ((this->runCounter & 1) ||
         (timeUTC() < this->startTime + this->historyDuration)))
        if ((this->runCounter & 2) &&
            ((this->lastAccess + this->recoveryDuration) < timeUTC()))
            queryActual();
        else
            queryNormal();
    else
        queryHistroy();
}

void LogSource::queryActual()
{
    logger.debug("Log Source: [" + this->params.name + "] route actual");
    time_t accessTimeFrom = std::max(
        timeUTC() - this->accessDuration,
        this->lastAccess);
    time_t accessTimeTo = timeUTC();

    const Request::Result result = getLogs_betweenTimes(accessTimeFrom, accessTimeTo);

    if (result.throttleTime == 0)
        this->logInterpreter->interpretJsonStr(result.data, this->params.timeField);
    else
        handleThrottle(result.throttleTime);
}

void LogSource::queryNormal()
{
    logger.debug("Log Source: [" + this->params.name + "] route normal");
    time_t accessTimeFrom = this->lastAccess;

    time_t accessTimeTo = std::min(
        timeUTC(),
        this->lastAccess + this->recoveryDuration);

    const Request::Result result = getLogs_betweenTimes(accessTimeFrom, accessTimeTo);

    if (result.throttleTime == 0)
    {
        this->logInterpreter->interpretJsonStr(result.data, this->params.timeField);
        this->lastAccess =
            std::max(
                this->startTime,
                std::min(
                    accessTimeTo,
                    timeUTC() + this->accessInterval - this->accessDuration));

        if (!this->isdHistoryAccessEnabled)
            this->logInterpreter->cleareCacheBefore(this->lastAccess);
    }
    else
        handleThrottle(result.throttleTime);
}

void LogSource::queryHistroy()
{
    logger.debug("Log Source: [" + this->params.name + "] route history");
    time_t accessTimeFrom = this->lastHistoryAccess;
    time_t accessTimeTo = std::min(
        this->lastHistoryAccess + this->historyAccessDuration,
        this->lastAccess);

    const Request::Result result = getLogs_betweenTimes(accessTimeFrom, accessTimeTo);

    if (result.throttleTime == 0)
    {
        this->logInterpreter->interpretJsonStr(result.data, this->params.timeField);

        if (accessTimeTo < this->lastAccess)
        {
            this->lastHistoryAccess = accessTimeTo;
        }
        else // reached present
        {
            this->lastHistoryAccess = std::max(
                timeUTC() - this->historyDuration,
                this->startTime);

            this->logInterpreter->cleareCacheBefore(this->lastHistoryAccess);
        }
    }
    else
        handleThrottle(result.throttleTime);
}

void LogSource::handleThrottle(const int throttleTime)
{
    logger.debug(
        this->params.name + ": got throttled for " + std::to_string(throttleTime) + "s");
    this->throttledUntil = timeUTC() + throttleTime;
}

void LogSource::run()
{
    if (!canRun())
    {
        return;
    }

    this->runCounter++;
    routeRun();
}
