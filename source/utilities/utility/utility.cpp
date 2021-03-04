#include "utility.hpp"

#include <string>
#include <ctime>

std::string timestampToDate(time_t timeStamp, const char *format)
{
    char dateBuffer[DATE_BUFFER_LEN];
    strftime(dateBuffer, DATE_BUFFER_LEN, format, localtime(&timeStamp));

    return std::string(dateBuffer);
}

std::string toLowerCase(std::string str)
{
    std::string result;
    std::transform(
        str.begin(),
        str.end(),
        str.begin(),
        [](unsigned char c) {
            return std::tolower(c);
        });

    return str;
}
