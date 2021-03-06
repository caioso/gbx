#include "MessageHandler.h"

using namespace gbx::interfaces;
using namespace std;
using namespace gbx;

namespace gbx::runtime
{

MessageHandler::MessageHandler(std::shared_ptr<interfaces::ServerTransport> transport)
    : _transport(transport)
{}

void MessageHandler::Initialize()
{
    _transport->Subscribe(shared_from_this());
}

void MessageHandler::Notify(std::shared_ptr<gbxcommons::NotificationArguments>)
{}

}