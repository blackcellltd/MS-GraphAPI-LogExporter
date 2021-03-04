#include "logger.hpp"

#include "../../utilities/utility/utility.hpp"

void Logger::log(Level level, const std::string &text)
{
    this->logLock.lock();

    if (level <= this->loglevel)
        *(this->stream)
            << this->levelMetaData[level].prefix
            << " ["
            << getTimeStampUTC()
            << "] "
            << text
            << std::endl;

    this->logLock.unlock();
}
