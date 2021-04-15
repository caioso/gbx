#pragma once

namespace gbxcore::interfaces
{

class VideoControllerInterface
{
public:
    virtual ~VideoControllerInterface() = default;

    virtual void EnableVideo() = 0;
    virtual void DisableVideo() = 0;
};

}