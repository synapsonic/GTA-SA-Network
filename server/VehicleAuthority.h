#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>

namespace gtanet {
struct VehicleAuthorityState {
    uint32_t ownerPlayerId=0;
    uint32_t driverPlayerId=0;
    uint32_t revision=0;
};
class VehicleAuthority {
    std::mutex m_;
    std::unordered_map<uint32_t,VehicleAuthorityState> vehicles_;
public:
    bool Claim(uint32_t vehicleId,uint32_t playerId,int8_t seat,VehicleAuthorityState& out) {
        std::lock_guard<std::mutex> l(m_);
        auto &v=vehicles_[vehicleId];
        if (seat==0) {
            if (v.driverPlayerId!=0 && v.driverPlayerId!=playerId) return false;
            v.driverPlayerId=playerId; v.ownerPlayerId=playerId; ++v.revision;
        }
        out=v; return true;
    }
    bool CanPublish(uint32_t vehicleId,uint32_t playerId) {
        std::lock_guard<std::mutex> l(m_);
        auto i=vehicles_.find(vehicleId);
        return i!=vehicles_.end() && i->second.ownerPlayerId==playerId;
    }
    bool Exit(uint32_t vehicleId,uint32_t playerId,VehicleAuthorityState& out) {
        std::lock_guard<std::mutex> l(m_);
        auto i=vehicles_.find(vehicleId); if(i==vehicles_.end()) return false;
        if(i->second.driverPlayerId==playerId) {
            i->second.driverPlayerId=0;
            i->second.ownerPlayerId=0;
            ++i->second.revision;
        }
        out=i->second; return true;
    }
    void Disconnect(uint32_t playerId) {
        std::lock_guard<std::mutex> l(m_);
        for(auto &[id,v]:vehicles_) if(v.ownerPlayerId==playerId || v.driverPlayerId==playerId) {
            if(v.ownerPlayerId==playerId) v.ownerPlayerId=0;
            if(v.driverPlayerId==playerId) v.driverPlayerId=0;
            ++v.revision;
        }
    }
};
}