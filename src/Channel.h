#pragma once

#include <functional>
#include <memory>

#include "GBXExceptions.h"

namespace gbx
{

enum class ChannelType
{
    In,
    Out,
    InOut
};

template<typename T>
class Channel : public std::enable_shared_from_this<Channel<T>>
{
public:
    Channel(ChannelType type)
    : _type(type)
    {}

    ~Channel() = default;

    ChannelType Type()
    {
        return _type;
    }
    
    void Bind(std::shared_ptr<Channel<T>> binding)
    {
        _binding = binding;
        binding->_binding = this->shared_from_this();
    }

    void OnReceived(const std::function<void(T)>& callback)
    {
        if (_type == ChannelType::In)
            throw ChannelException("input channel cannot receive messages");

        _callback = callback;
    }

    void Send(T message)
    {
        if (_type == ChannelType::Out)
            throw ChannelException("output channel cannot send messages");

        if (_binding == nullptr)
            throw ChannelException("no binding has been confugured");

        if (_binding->_callback == nullptr)
            throw ChannelException("bounded channel has not OnReceive callback configured");

        _binding->_callback(message);
    }

protected:
    ChannelType _type;
    std::shared_ptr<Channel<T>> _binding;
    std::function<void(T)> _callback;
};

}