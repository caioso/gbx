#pragma once

#include <string>

namespace gbxasm::interfaces
{

class StreamSink
{
public:
    virtual ~StreamSink() = default;
    
    virtual void Write(std::string) = 0;
};

}