#pragma once
#include <cstdint>
#include "CombatBridge.h"
namespace gtanet {
class CombatSyncController {
    CombatBridge bridge_;
    uint64_t lastAimMs_=0; uint32_t aimSeq_=0,shotSeq_=0;
public:
    template<class Send> void Tick(uint32_t playerId,uint64_t nowMs,Send&& send){
        auto s=bridge_.ReadLocal();
        if(nowMs-lastAimMs_>=50){
            AimStatePacket p{}; p.playerId=playerId;p.weapon=s.weapon;p.aiming=s.aiming?1:0;
            p.ox=s.ox;p.oy=s.oy;p.oz=s.oz;p.dx=s.dx;p.dy=s.dy;p.dz=s.dz;p.sequence=++aimSeq_;
            send(&p,sizeof(p));lastAimMs_=nowMs;
        }
        if(s.fired){
            ShotPacket p{};p.playerId=playerId;p.shotId=++shotSeq_;p.weapon=s.weapon;
            p.ox=s.ox;p.oy=s.oy;p.oz=s.oz;p.dx=s.dx;p.dy=s.dy;p.dz=s.dz;p.clientTimeMs=nowMs;
            send(&p,sizeof(p));
        }
    }
};
}