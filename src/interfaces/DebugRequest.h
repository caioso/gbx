#pragma once

#include <cstdlib>

namespace gbx::interfaces
{

enum class RequestType
{
    // Data Requests
    Register,
    UnknownType
};

class DebugRequest
{
public:
    DebugRequest(RequestType);
    virtual ~DebugRequest() = default;

protected:
    RequestType _requestType;
};

}