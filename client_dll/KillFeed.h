#pragma once
#include <deque>
#include <string>
#include <cstdint>
#include "../shared/Protocol.h"
namespace gtanet {
struct KillFeedEntry { uint32_t killer=0,victim=0; uint8_t weapon=0; uint64_t receivedMs=0; };
class KillFeed {
    std::deque<KillFeedEntry> entries_;
public:
    void Push(const KillFeedPacket&p,uint64_t now){entries_.push_front({p.killerId,p.victimId,p.weapon,now});while(entries_.size()>6)entries_.pop_back();}
    void Expire(uint64_t now){while(!entries_.empty()&&now-entries_.back().receivedMs>8000)entries_.pop_back();}
    const auto& Entries()const{return entries_;}
};
}