#pragma once

class EndpointAble
{
public:
    virtual void run() = 0;
    virtual bool canRun() = 0;
};