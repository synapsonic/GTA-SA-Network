#pragma once
#include <string>
#include <cstdint>
namespace gtanet {
class Resource {
public:
    virtual ~Resource()=default;
    virtual const char* Name()const=0;
    virtual bool Start()=0;
    virtual void Stop()=0;
    virtual void Tick(uint64_t nowMs){(void)nowMs;}
};
}