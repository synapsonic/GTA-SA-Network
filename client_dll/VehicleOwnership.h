#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>
namespace gtanet {
class VehicleOwnership {
    std::mutex m_;
    struct Owner { uint32_t player=0, revision=0; };
    std::unordered_map<uint32_t,Owner> owners_;
public:
    void Apply(uint32_t vehicle,uint32_t player,uint32_t revision) {
        std::lock_guard<std::mutex> l(m_);
        auto &o=owners_[vehicle]; if(revision>=o.revision) o={player,revision};
    }
    bool IsOwner(uint32_t vehicle,uint32_t player) {
        std::lock_guard<std::mutex> l(m_);
        auto i=owners_.find(vehicle); return i!=owners_.end() && i->second.player==player;
    }
};
}