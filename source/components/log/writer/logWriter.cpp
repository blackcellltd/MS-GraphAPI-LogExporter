#include "logWriter.hpp"

#include <sstream>

#include "../../../utilities/runtimeError.hpp"
#include "../../../providers/logger/logger.hpp"
#include "../../../utilities/utility/utility.hpp"

void LogWriter::startBuffer()
{
    this->bufferLock.lock();
    this->buffer.clear();
}

void LogWriter::addBuffer(const Json::Value &value)
{
    this->buffer.push_back(value);
}

void LogWriter::writeBuffer()
{
    if (!(this->buffer.empty()))
    {
        Json::StreamWriterBuilder builder;
        std::shared_ptr<Json::StreamWriter> writer = std::shared_ptr<Json::StreamWriter>(builder.newStreamWriter());

        std::string filePath = getNextFileName();
        std::ofstream fileStream(filePath);
        if (!(fileStream.is_open()))
            throw RunntimeError("Log Writer: failed to open file", "filePath: " + filePath);

        logger.information("Log Writer: writing out " + std::to_string(this->buffer.size()) + " logs, to " + filePath);

        writeOutBuffer(writer, fileStream);

        fileStream.close();
        this->buffer.clear();
    }

    this->bufferLock.unlock();
}

void LogWriter::writeOutBuffer(std::shared_ptr<Json::StreamWriter> writer, std::ofstream &fileStream)
{
    fileStream << "[";
    bool isFirst = true;
    for (const Json::Value &value : this->buffer)
    {
        if (isFirst)
            isFirst = false;
        else
            fileStream << ",";
        writer->write(value, &fileStream);
    }
    fileStream << "]";
}

std::string LogWriter::getNextFileName()
{
    return this->fileName + "-" + std::to_string(this->sequenceNum++) + "-" + getTimeStampUTC() + ".json";
}
