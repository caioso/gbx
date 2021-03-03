#include "RequestTranslator.h"

using namespace std;

namespace gbx
{

RequestTranslator::RequestTranslator()
{}

void RequestTranslator::Notify(std::shared_ptr<interfaces::RawRequestEventArgs> notify)
{
    //auto request = make_shared<RequestEventArgs>();
    for(auto observer : _observers)
        observer->Notify(nullptr);
}

void RequestTranslator::AddEventListener(std::shared_ptr<gbxcommons::Observer<RequestEventArgs>> observer)
{
    _observers.push_back(observer);
}

}