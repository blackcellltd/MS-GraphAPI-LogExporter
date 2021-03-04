#include "timmer.hpp"

#include <iostream>
#include <cstring>
#include <string>
#include <vector>

#include "../../utilities/runtimeError.hpp"
#include "../../providers/logger/logger.hpp"

namespace Timmer
{
    static sigevent signalEvent;
    static timer_t timerId;
    static itimerspec interval;
    static std::vector<std::shared_ptr<Timmerable>> targets;
    static bool started = false;
    static size_t counter = 0;

    static void initSignalEvent();

    static void sigHandler(int val)
    {
        (void)val;
        targets[(counter++) % targets.size()]->signal();
    }

    static bool wasInit = false;
    void init()
    {
        std::signal(TIMMER_SIGNAL, sigHandler);

        initSignalEvent();
        if (timer_create(CLOCK_REALTIME, &signalEvent, &timerId) == 0)
            logger.success("Timmer: created successfully");
        else
            throw RunntimeError("Timmer creation failed");

        wasInit = true;
    }

    void start(int intervalSec)
    {
        if (!wasInit)
            throw(RunntimeError("##DEVELOPMENT## Timmer: was not initialized"));

        long int devidedIntervalSec = intervalSec / targets.size() / 2;

        interval = {{devidedIntervalSec, 0}, {devidedIntervalSec, 0}};

        if (timer_settime(timerId, 0, &interval, NULL) == 0)
        {
            logger.success("Timmer: started");
            started = true;
        }
        else
            throw(RunntimeError("Timmer: settime was unsuccessful"));

        sigHandler(0);
    }

    void addTarget(std::shared_ptr<Timmerable> target)
    {
        if (started)
            throw(RunntimeError("##DEVELOPMENT## Timmer: already started"));
        targets.push_back(target);
    }

    static void initSignalEvent()
    {
        memset(&signalEvent, 0, sizeof(signalEvent));

        signalEvent.sigev_notify = SIGEV_SIGNAL;
        signalEvent.sigev_signo = TIMMER_SIGNAL;
        signalEvent.sigev_value.sival_int = 1;
    }

} // namespace Timmer