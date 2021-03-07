#include "DebugMessageNotificationArguments.h"

using namespace gbx::interfaces;
using namespace std;

namespace gbx::runtime
{

DebugMessageNotificationArguments::DebugMessageNotificationArguments(shared_ptr<DebugMessage> message)
    : _debugMessage(message)
{}

shared_ptr<DebugMessage> DebugMessageNotificationArguments::Message()
{
    return _debugMessage;
}

}