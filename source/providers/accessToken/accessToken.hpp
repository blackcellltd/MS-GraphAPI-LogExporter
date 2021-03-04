#pragma once

#include <string>
#include <ctime>
#include <mutex>

class AccessToken
{
private:
    std::mutex tokenLock;
    time_t expressAt = 0;
    std::string token = "";
    std::string tokenType = "";

public:
    AccessToken();
    std::string getToken();

private:
    void handleSuccessFullRequest(const std::string &responseText);
    void handleFailedRequest();
    void newToken();
};