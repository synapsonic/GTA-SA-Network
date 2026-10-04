#include "PluginSdkBridge.h"
#include "GtaAdapter.h"
#ifdef GTANET_WITH_PLUGIN_SDK
#include <plugin.h>
#include <game_sa/CPlayerPed.h>
#include <game_sa/CWorld.h>
#include <game_sa/CVector.h>
namespace { GtaAdapter* g_adapter=nullptr; }
bool PluginSdkBridge::Install(GtaAdapter* a){
    g_adapter=a;
    plugin::Events::gameProcessEvent += [](){ if(g_adapter) g_adapter->TickRemotePlayers(1.0f/60.0f); };
    return true;
}
void PluginSdkBridge::Uninstall(){ g_adapter=nullptr; }
bool PluginSdkBridge::Available(){ return true; }
#else
bool PluginSdkBridge::Install(GtaAdapter*){ return false; }
void PluginSdkBridge::Uninstall(){}
bool PluginSdkBridge::Available(){ return false; }
#endif
