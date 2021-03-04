#pragma once

#include <string>
#include <memory>

#include "../../endpoint/endpointAble.hpp"
#include "../../../providers/accessToken/accessToken.hpp"
#include "../interpreter/logInterpreter.hpp"
#include "../../../utilities/request/request.hpp"
#include "../../../providers/config/config.hpp"
#include "../../../utilities/utility/utility.hpp"

class LogSource : public EndpointAble
{
public:
    struct Parameters
    {
        const std::string name;
        const std::string apiEndpoint;
        const std::string timeField;

        Parameters(
            const std::string &_name,
            const std::string &_apiEndpoint,
            const std::string &_timeField)
            : name(_name),
              apiEndpoint(_apiEndpoint),
              timeField(_timeField) {}
    };

protected:
    std::shared_ptr<AccessToken> token = NULL;
    std::shared_ptr<LogInterpreter> logInterpreter = NULL;
    Parameters params;
    time_t throttledUntil = 0;
    time_t lastAccess = timeUTC();
    time_t lastHistoryAccess = timeUTC();
    const time_t startTime = timeUTC();
    int runCounter = 0;

    const time_t accessDuration = config.getInt("Access Duration");
    const time_t accessInterval = config.getInt("Access Intervals");
    const time_t accessDelay = config.getInt("Access Delay");
    const time_t recoveryDuration = config.getInt("Recovery Duration");
    const time_t historyDuration = config.getInt("History Duration");
    const time_t historyAccessDuration = config.getInt("History Access Duration");
    const bool isdHistoryAccessEnabled = config.getInt("History Access Duration") != 0;

public:
    LogSource(
        std::shared_ptr<AccessToken> _token,
        std::shared_ptr<LogInterpreter> _logInterpreter,
        const Parameters &_params)
        : token(_token),
          logInterpreter(_logInterpreter),
          params(_params) {}

    const Request::Result getLogs_betweenTimes(time_t from, time_t to);
    inline const Request::Result getLogs_from(time_t from)
    {
        return getLogs_betweenTimes(from, timeUTC());
    }

    void run();
    bool canRun() { return this->throttledUntil < timeUTC(); }

private:
    inline time_t addDelay(time_t value) const
    {
        return value - this->accessDelay;
    }
    bool needRecovery();
    inline bool isNeedQuerryAct()
    {
        return (this->lastAccess + this->recoveryDuration < timeUTC()) &&
               ((this->runCounter++) & 1);
    }
    void queryHistroy();
    void queryActual();
    void queryNormal();

    void handleThrottle(const int throttleTime);

    void routeRun();
};