#include "CombatBridge.h"
namespace gtanet {
LocalCombatState CombatBridge::ReadLocal(){
    LocalCombatState s{};
#if defined(GTANET_WITH_PLUGIN_SDK)
    // Read weapon/aim/fire from the local CPed and camera using the selected plugin-sdk revision.
#endif
    return s;
}
void CombatBridge::ApplyRemoteAim(const AimStatePacket&){
#if defined(GTANET_WITH_PLUGIN_SDK)
    // Drive the remote ped's visual aim task only. Damage is server-authoritative.
#endif
}
void CombatBridge::PlayRemoteShot(const ShotPacket&){
#if defined(GTANET_WITH_PLUGIN_SDK)
    // Play muzzle/weapon visual effects for the remote ped.
#endif
}
void CombatBridge::ApplyHealth(const PlayerHealthPacket&){
#if defined(GTANET_WITH_PLUGIN_SDK)
#endif
}
void CombatBridge::ApplyDeath(const PlayerDeathPacket&){
#if defined(GTANET_WITH_PLUGIN_SDK)
#endif
}
void CombatBridge::ApplyRespawn(const PlayerRespawnPacket&){
#if defined(GTANET_WITH_PLUGIN_SDK)
#endif
}
}