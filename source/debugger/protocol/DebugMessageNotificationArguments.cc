#include "DebugMessageNotificationArguments.h"

using namespace gbxdb;
using namespace gbxdb::interfaces;
using namespace std;

namespace gbxdb::protocol                     
{

DebugMessageNotificationArguments::DebugMessageNotificationArguments(shared_ptr<DebugMessage> message)
    : _debugMessage(message)
{}

shared_ptr<DebugMessage> DebugMessageNotificationArguments::Message()
{
    return _debugMessage;
}

}