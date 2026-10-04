#include "ClientCapabilities.h"
#include "PluginSdkBridge.h"
namespace gtanet {
std::string ClientCapabilities::Summary()const{
 auto f=[](bool v){return v?"yes":"no";};
 return "plugin-sdk="+std::string(f(pluginSdk))+
 ", game-thread="+f(gameThread)+", local-player="+f(localPlayer)+
 ", local-vehicle="+f(localVehicle)+", remote-peds="+f(remotePeds)+
 ", remote-vehicles="+f(remoteVehicles)+", remote-seats="+f(remoteSeats);
}
ClientCapabilities DetectClientCapabilities(){
 ClientCapabilities c;
#if defined(GTANET_WITH_PLUGIN_SDK)
 c.pluginSdk=PluginSdkBridge::Available();c.gameThread=c.pluginSdk;c.localPlayer=c.pluginSdk;
 c.localVehicle=c.pluginSdk;
 #if defined(GTANET_PLUGINSDK_PED_FACTORY)
 c.remotePeds=true;
 #endif
 #if defined(GTANET_PLUGINSDK_VEHICLE_FACTORY)
 c.remoteVehicles=true;
 #endif
 #if defined(GTANET_PLUGINSDK_SEAT_API)
 c.remoteSeats=true;
 #endif
#endif
 return c;
}
}