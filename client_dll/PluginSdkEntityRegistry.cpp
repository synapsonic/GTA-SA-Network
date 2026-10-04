#include "PluginSdkEntityRegistry.h"
#if defined(GTANET_WITH_PLUGIN_SDK) && defined(GTANET_PLUGINSDK_SEAT_API)
#include <game_sa/CPed.h>
#include <game_sa/CVehicle.h>
namespace gtanet {
// Exact task creation differs across plugin-sdk revisions.
// Provide these two functions in PluginSdkSeatApi.cpp for the checkout used by the Windows build.
extern bool GtaNetAssignPedSeat(CPed*,CVehicle*,int8_t);
extern void GtaNetRemovePedFromVehicle(CPed*,CVehicle*);
bool PluginSdkEntityRegistry::AssignSeat(void*p,void*v,int8_t s){return GtaNetAssignPedSeat((CPed*)p,(CVehicle*)v,s);}
void PluginSdkEntityRegistry::RemoveFromVehicle(void*p,void*v){GtaNetRemovePedFromVehicle((CPed*)p,(CVehicle*)v);}
}
#endif
