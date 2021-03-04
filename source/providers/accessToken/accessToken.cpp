#include "accessToken.hpp"

#include <string>
#include <json/json.h>
#include <bits/stdc++.h>
#include <ctime>

#include "../config/config.hpp"
#include "../../utilities/request/request.hpp"
#include "../logger/logger.hpp"
#include "../../utilities/runtimeError.hpp"

AccessToken::AccessToken()
{
    newToken();
}

void AccessToken::newToken()
{
    std::string postStr =
        "grant%5Ftype=client%5Fcredentials&client%5Fid=" +
        config.getString("Client Id") +
        "&scope=https%3A%2F%2Fgraph%2Emicrosoft%2Ecom%2F%2Edefault&client%5Fsecret=" +
        config.getString("Client Secret");
    std::string urlStr =
        "https://login.microsoftonline.com/" +
        config.getString("Tenant Id") +
        "/oauth2/v2.0/token";

    this->tokenLock.lock();

    Request::Result response = Request()
                                   .setMethodPost()
                                   .setUrl(urlStr)
                                   .setFormData(postStr)
                                   .exec();

    if (response.throttleTime == 0)
        handleSuccessFullRequest(response.data);
    else
        handleFailedRequest();

    this->tokenLock.unlock();
}

void AccessToken::handleSuccessFullRequest(const std::string &responseText)
{
    Json::Value data;
    std::stringstream(responseText) >> data;

    this->expressAt = time(NULL) + (data["expires_in"].isNull()
                                        ? throw RunntimeError("Acces Token: error in fetching fields (expires_in)")
                                        : data["expires_in"].asInt());

    this->token = data["access_token"].isNull()
                      ? throw RunntimeError("Acces Token: error in fetching fields (access_token)")
                      : data["access_token"].asString();
}

void AccessToken::handleFailedRequest()
{
    this->expressAt = 0;
    this->token = "";
}

std::string AccessToken::getToken()
{
    if (time(NULL) >= this->expressAt)
        newToken();

    return this->token;
}