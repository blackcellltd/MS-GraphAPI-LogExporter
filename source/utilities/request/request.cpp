#include "request.hpp"

#include <string>
#include <iostream>
#include <curl/curl.h>

#include "../../providers/logger/logger.hpp"
#include "../../providers/config/config.hpp"
#include "../runtimeError.hpp"

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}
Request::Request()
{
    if ((this->curl = curl_easy_init()) == NULL)
        throw RunntimeError("Error in inicializing Curl object");

    curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &(this->resultData));

    curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, WriteCallback);
    curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &(this->resultHeader));

    curl_easy_setopt(this->curl, CURLOPT_ERRORBUFFER, this->errorBuffer);

    if (logger.isLogLevel(Logger::Level::Request))
        curl_easy_setopt(this->curl, CURLOPT_VERBOSE, 1L);
}

Request::~Request()
{
    curl_slist_free_all(this->headders);
    curl_easy_cleanup(this->curl);
}

template <typename T>
Request &Request::setOption(CURLoption option, T parameter)
{
    curl_easy_setopt(this->curl, option, parameter);

    return *this;
}

Request &Request::setMethodPost()
{
    curl_easy_setopt(this->curl, CURLOPT_POST, 1);

    return *this;
}

Request &Request::setMethodGet()
{
    curl_easy_setopt(this->curl, CURLOPT_HTTPGET, 1);

    return *this;
}

Request &Request::setFormData(const std::string &text)
{
    curl_easy_setopt(this->curl, CURLOPT_POSTFIELDSIZE, text.length());
    curl_easy_setopt(this->curl, CURLOPT_COPYPOSTFIELDS, text.c_str());

    return *this;
}

Request &Request::setUrl(const std::string &url)
{
    curl_easy_setopt(this->curl, CURLOPT_URL, url.c_str());
    this->url = url;

    return *this;
}

Request &Request::setBearerAccessToken(const std::string &token)
{
    curl_easy_setopt(this->curl, CURLOPT_XOAUTH2_BEARER, token.c_str());

    return *this;
}

std::string Request::desprected_exec()
{
    return exec().data;
}

std::string Request::getHeaderField(const std::string &fieldName) const
{
    size_t startPoz;
    size_t endPoz;

    if ((startPoz = this->resultHeader.find(fieldName)) >= this->resultHeader.length())
        throw RunntimeError(
            "Development Error: searched filed is not present [" + fieldName + "]");

    startPoz += fieldName.length() + std::string(": ").length();

    if ((endPoz = this->resultHeader.find('\r', startPoz)) >= this->resultHeader.length())
        throw RunntimeError(
            "Development Error: searched filed is not present [" + fieldName + "] (end of line)");

    return this->resultHeader.substr(startPoz, (endPoz - startPoz));
}

const Request::Result Request::exec()
{

    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, this->headders);
    this->res = curl_easy_perform(this->curl);

    if (this->res != CURLE_OK)
    {
        logger.error("Curl error: " + std::string(this->errorBuffer));
        return Request::Result(config.getInt("Access Intervals"), "");
    }

    logger.request(this->resultHeader);
    logger.request(this->resultData);

    int responseStatusCode;
    curl_easy_getinfo(this->curl, CURLINFO_RESPONSE_CODE, &responseStatusCode);

    double requestTime = 0;
    curl_easy_getinfo(this->curl, CURLINFO_TOTAL_TIME, &requestTime);
    logger.debug(
        "Request: time: " +
        std::to_string(requestTime) +
        "s, response size: " +
        std::to_string(this->resultData.length()) +
        ", url: " + this->url);

    if (responseStatusCode >= 200 && responseStatusCode < 300)
        return Request::Result(0, this->resultData);

    if (responseStatusCode == 429)
        return Request::Result(
            std::stoi(getHeaderField("Retry-After")),
            this->resultData);

    logger.error("Unsuccesful request status code: " +
                 std::to_string(responseStatusCode) +
                 " url: " + this->url);
    logger.debug(this->resultHeader);
    logger.debug(this->resultData);

    return Request::Result(config.getInt("Access Intervals"), "");
}

Request &Request::addHeadderFild(const std::string &field)
{
    struct curl_slist *tmpHeadders = curl_slist_append(this->headders, field.c_str());

    if (tmpHeadders == NULL)
    {
        logger.error("Erro in adding headder fild: " + field);
        return *this;
    }

    this->headders = tmpHeadders;

    return *this;
}
