#pragma once

#include <curl/curl.h>
#include <string>

#include "../../providers/logger/logger.hpp"

class Request
{
public:
    struct Result
    {
        const int throttleTime;
        const std::string data;

        Result(
            const int _throttleTime,
            const std::string &_data)
            : throttleTime(_throttleTime),
              data(_data) {}
    };

private:
    CURL *curl = NULL;
    CURLcode res;
    struct curl_slist *headders = NULL;
    char errorBuffer[CURL_ERROR_SIZE] = "";
    std::string url;
    std::string resultHeader;
    std::string resultData;

public:
    Request();
    ~Request();

    template <typename T>
    Request &setOption(CURLoption option, T parameter);

    Request &setMethodPost();
    Request &setMethodGet();
    Request &setFormData(const std::string &text);
    Request &setUrl(const std::string &url);
    Request &setBearerAccessToken(const std::string &token);

    Request &addHeadderFild(const std::string &field);

    std::string desprected_exec();
    const Result exec();
    std::string getResultData()
    {
        return this->resultData;
    }

private:
    std::string getHeaderField(const std::string &fieldName) const;
};