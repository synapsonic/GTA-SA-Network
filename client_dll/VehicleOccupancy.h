#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>
namespace gtanet {
struct SeatAssignment { uint32_t vehicleId=0; int8_t seat=-1; };
class VehicleOccupancy {
    std::mutex m_;
    std::unordered_map<uint32_t,SeatAssignment> byPlayer_;
public:
    void Enter(uint32_t player,uint32_t vehicle,int8_t seat){ std::lock_guard<std::mutex> l(m_); byPlayer_[player]={vehicle,seat}; }
    void Exit(uint32_t player){ std::lock_guard<std::mutex> l(m_); byPlayer_.erase(player); }
    bool Get(uint32_t player,SeatAssignment& out){ std::lock_guard<std::mutex> l(m_); auto i=byPlayer_.find(player); if(i==byPlayer_.end())return false; out=i->second; return true; }
};
}