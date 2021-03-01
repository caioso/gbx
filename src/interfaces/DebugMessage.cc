#include "DebugMessage.h"

namespace gbx::interfaces
{

DebugMessage::DebugMessage(MessageType type)
    : _requestType(type)    
{}
    
MessageType DebugMessage::Type()
{
    return _requestType;
}

}