#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <json/json.h>
#include <mutex>
#include <memory>

class LogWriter
{
private:
    std::string fileName;
    int sequenceNum = 0;
    std::vector<Json::Value> buffer;
    std::mutex bufferLock;

public:
    LogWriter(std::string _fileName) : fileName(_fileName) {}

    void startBuffer();
    void addBuffer(const Json::Value &value);
    void writeBuffer();

private:
    void writeOutBuffer(std::shared_ptr<Json::StreamWriter> writer, std::ofstream &fileStream);
    std::string getNextFileName();
};