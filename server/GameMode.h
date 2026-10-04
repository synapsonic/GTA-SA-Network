#pragma once
#include <cstdint>
#include <string>
namespace gtanet {
class GameMode {
public:
    virtual ~GameMode()=default;
    virtual const char* Name()const=0;
    virtual void OnStart(){}
    virtual void OnStop(){}
    virtual void OnTick(uint64_t nowMs){(void)nowMs;}
    virtual void OnPlayerConnect(uint32_t player){(void)player;}
    virtual void OnPlayerDisconnect(uint32_t player){(void)player;}
    virtual void OnPlayerDeath(uint32_t victim,uint32_t killer,uint8_t weapon){(void)victim;(void)killer;(void)weapon;}
    virtual bool OnCommand(uint32_t player,const std::string& command){(void)player;(void)command;return false;}
};
}