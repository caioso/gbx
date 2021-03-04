#include "RequestTranslator.h"

using namespace std;
using namespace gbx::requests;

namespace gbx
{

void RequestTranslator::Notify(shared_ptr<interfaces::RawDebugMessageEventArgs> rawRequest)
{
    auto debugMessage = DecodeRawRequest(rawRequest->RawRequest());
    
    for(auto observer : _observers)
        observer.lock()->Notify(debugMessage);
}

void RequestTranslator::AddEventListener(weak_ptr<gbxcommons::Observer<DebugMessageEventArgs>> observer)
{
    _observers.push_back(observer);
}

std::shared_ptr<DebugMessageEventArgs> RequestTranslator::DecodeRawRequest(std::array<uint8_t, interfaces::MaxRawRequestSize> requestBytes)
{
    auto messageId = requestBytes[1] << 0x08 | requestBytes[0];
    
    switch (messageId)
    {
        case MessageID::ClientConnectedMessage: return make_shared<DebugMessageEventArgs>(ClientConnectedRequest::MakeRequest(requestBytes));
        case MessageID::DebugServerStatusRequest: return make_shared<DebugMessageEventArgs>(DebugServerStatusRequest::MakeRequest(requestBytes));
    }

    return nullptr;
}

DebugMessageEventArgs::DebugMessageEventArgs(shared_ptr<interfaces::DebugMessage> message)
    : _message(message)
{}

shared_ptr<interfaces::DebugMessage> DebugMessageEventArgs::Message()
{
    return _message;
}

}