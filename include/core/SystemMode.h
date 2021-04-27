#pragma once

namespace gbxcore
{

enum class PrivilegeMode
{
    System,
    User,
    Both,
};

enum class SystemMode
{
    DMGBC,
    GBX
};

typedef PrivilegeMode SecurityLevel;
typedef SystemMode CPUMode;

}