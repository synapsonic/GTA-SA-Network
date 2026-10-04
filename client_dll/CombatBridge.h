#pragma once
#include <cstdint>
#include "../shared/Protocol.h"
namespace gtanet {
struct LocalCombatState { bool aiming=false; bool fired=false; uint8_t weapon=0; float ox=0,oy=0,oz=0,dx=0,dy=0,dz=0; };
class CombatBridge {
public:
    LocalCombatState ReadLocal();
    void ApplyRemoteAim(const AimStatePacket&);
    void PlayRemoteShot(const ShotPacket&);
    void ApplyHealth(const PlayerHealthPacket&);
    void ApplyDeath(const PlayerDeathPacket&);
    void ApplyRespawn(const PlayerRespawnPacket&);
};
}