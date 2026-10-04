#pragma once
#include <cstdint>
#include <string>
#include <functional>
namespace gtanet {
struct ScriptApi {
    std::function<void(const std::string&)> broadcast;
    std::function<void(uint32_t,const std::string&)> messagePlayer;
    std::function<void(uint32_t,float,float,float,float)> setPlayerPos;
    std::function<void(uint32_t,float)> setPlayerHealth;
    std::function<uint32_t(uint16_t,float,float,float,float)> createVehicle;
    std::function<void(uint32_t)> destroyVehicle;
};
}