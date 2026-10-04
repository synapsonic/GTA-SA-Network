#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
namespace gtanet {
class RespawnManager {
    std::unordered_map<uint32_t,uint64_t> due_;
public:
    void Schedule(uint32_t player,uint64_t nowMs,uint64_t delayMs=5000){due_[player]=nowMs+delayMs;}
    std::vector<uint32_t> Due(uint64_t nowMs){
        std::vector<uint32_t> out;
        for(auto it=due_.begin();it!=due_.end();) if(it->second<=nowMs){out.push_back(it->first);it=due_.erase(it);}else ++it;
        return out;
    }
    void Remove(uint32_t p){due_.erase(p);}
};
}