#pragma once
#include "generated/tsapi.hpp"

class ExtEventCoreService : public tsapi::AbstractEventCore {
public:
  ExtEventCoreService(tsapi::RpcTransport &transport) : AbstractEventCore(transport) {}
};
