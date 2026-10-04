#include "RemoteSeatAdapter.h"
#include "VehicleIdRegistry.h"
#if defined(GTANET_WITH_PLUGIN_SDK) && defined(GTANET_PLUGINSDK_SEAT_API)
#include "PluginSdkEntityRegistry.h"
#include <game_sa/CPed.h>
#include <game_sa/CVehicle.h>
#endif
namespace gtanet {
bool RemoteSeatAdapter::PutRemotePedInVehicle(uint32_t playerId,uint32_t vehicleId,int8_t seat){
#if defined(GTANET_WITH_PLUGIN_SDK) && defined(GTANET_PLUGINSDK_SEAT_API)
 auto*ped=static_cast<CPed*>(PluginSdkEntityRegistry::Instance().Ped(playerId));
 auto*v=static_cast<CVehicle*>(VehicleIdRegistry::Instance().Handle(vehicleId));
 if(!ped||!v)return false;
 return PluginSdkEntityRegistry::Instance().AssignSeat(ped,v,seat);
#else
 (void)playerId;(void)vehicleId;(void)seat;return false;
#endif
}
void RemoteSeatAdapter::RemoveRemotePedFromVehicle(uint32_t playerId,uint32_t vehicleId){
#if defined(GTANET_WITH_PLUGIN_SDK) && defined(GTANET_PLUGINSDK_SEAT_API)
 auto*ped=PluginSdkEntityRegistry::Instance().Ped(playerId);auto*v=VehicleIdRegistry::Instance().Handle(vehicleId);
 if(ped&&v)PluginSdkEntityRegistry::Instance().RemoveFromVehicle(ped,v);
#else
 (void)playerId;(void)vehicleId;
#endif
}
}