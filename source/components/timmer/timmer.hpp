#pragma once

#include <csignal>
#include <memory>

namespace Timmer
{
#define TIMMER_SIGNAL (SIGUSR1)

    class Timmerable
    {
    public:
        virtual void signal() = 0;
    };

    extern void init();
    extern void start(int intervalSec);
    extern void addTarget( std::shared_ptr<Timmerable> target);

} // namespace Timmer
