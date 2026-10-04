#pragma once
#include <cstdint>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include "WeaponRules.h"
#include "LagCompensation.h"
namespace gtanet {
struct CombatPlayer { float x=0,y=0,z=0,health=100,armour=0; bool alive=true; uint64_t lastShotMs=0; };
struct DamageDecision { bool accepted=false; bool killed=false; float health=0,armour=0; };
class CombatAuthority {
    std::unordered_map<uint32_t,CombatPlayer> players_;
    static float MaxRange(uint8_t w){ if(w==34) return 350.f; if(w>=22&&w<=32) return 140.f; return 25.f; }
    static uint64_t MinShotInterval(uint8_t w){ if(w==34) return 900; if(w>=22&&w<=24) return 180; if(w>=25&&w<=27) return 650; return 80; }
public:
    CombatPlayer& Player(uint32_t id){ return players_[id]; }
    bool RegisterShot(uint32_t socketPlayerId,uint32_t packetPlayerId,uint8_t weapon,uint64_t nowMs){
        if(socketPlayerId!=packetPlayerId) return false;
        auto &p=players_[socketPlayerId]; if(!p.alive) return false;
        auto gap=MinShotInterval(weapon); if(p.lastShotMs && nowMs-p.lastShotMs<gap) return false;
        p.lastShotMs=nowMs; return true;
    }
    DamageDecision Damage(uint32_t socketPlayerId,uint32_t attackerId,uint32_t victimId,uint8_t weapon,float requested){
        DamageDecision d{}; if(socketPlayerId!=attackerId || attackerId==victimId) return d;
        auto ai=players_.find(attackerId), vi=players_.find(victimId);
        if(ai==players_.end()||vi==players_.end()||!ai->second.alive||!vi->second.alive) return d;
        float dx=ai->second.x-vi->second.x,dy=ai->second.y-vi->second.y,dz=ai->second.z-vi->second.z;
        if(std::sqrt(dx*dx+dy*dy+dz*dz)>MaxRange(weapon)) return d;
        float dmg=std::clamp(requested,0.0f,100.0f);
        float absorbed=std::min(vi->second.armour,dmg); vi->second.armour-=absorbed; dmg-=absorbed;
        vi->second.health=std::max(0.0f,vi->second.health-dmg);
        if(vi->second.health<=0){ vi->second.alive=false; d.killed=true; }
        d.accepted=true; d.health=vi->second.health; d.armour=vi->second.armour; return d;
    }
    void Respawn(uint32_t id,float x,float y,float z,float health=100){ auto &p=players_[id]; p.x=x;p.y=y;p.z=z;p.health=health;p.armour=0;p.alive=true;p.lastShotMs=0; }
    void Remove(uint32_t id){ players_.erase(id); }
};
}