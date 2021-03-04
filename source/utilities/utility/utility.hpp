#pragma once

#include <string>
#include <ctime>
#include <algorithm>

#define DATE_BUFFER_LEN (128)
#define DATE_DEFAULT_FORMAT ("%Y-%m-%dT%H:%M:%SZ")
std::string timestampToDate(time_t timeStamp, const char *format = DATE_DEFAULT_FORMAT);

#define TIME_SECOND (1)
#define TIME_MINUTE (60 * TIME_SECOND)
#define TIME_HOUR (60 * TIME_MINUTE)
#define TIME_5_MINUTES (TIME_MINUTE * 5)
#define TIME_10_MINUTES (TIME_MINUTE * 10)

inline time_t timeUTC()
{
    struct tm timeObj;
    time_t now = time(NULL);
    return mktime(gmtime_r(&now, &timeObj));
}
inline time_t timeUTC_floorMin()
{
    time_t time = timeUTC();
    return time - (time % TIME_MINUTE);
}

inline std::string getTimeStampUTC()
{
    return timestampToDate(timeUTC());
}

std::string toLowerCase(std::string str);