#include "GameThreadBridge.h"
#include "GtaAdapter.h"
#include "PluginSdkBridge.h"
#include <windows.h>
#include <fstream>
namespace {
void Log(const char* s){ std::ofstream("gtasa_network.log",std::ios::app)<<"[game-thread] "<<s<<"\n"; }
HANDLE timerHandle=nullptr, timerQueue=nullptr; GtaAdapter* gAdapter=nullptr;
VOID CALLBACK Tick(PVOID,BOOLEAN){ if(gAdapter) gAdapter->TickRemotePlayers(0.016f); }
}
bool GameThreadBridge::Install(GtaAdapter* adapter){
    if(installed_.exchange(true)) return true; adapter_=adapter; gAdapter=adapter;
    if(PluginSdkBridge::Available() && PluginSdkBridge::Install(adapter)){
        usingPluginSdk_=true; Log("plugin-sdk gameProcessEvent bridge installed."); return true;
    }
    timerQueue=CreateTimerQueue();
    if(!timerQueue || !CreateTimerQueueTimer(&timerHandle,timerQueue,Tick,nullptr,16,16,WT_EXECUTEDEFAULT)){
        installed_=false; gAdapter=nullptr; Log("Failed to install fallback tick bridge"); return false;
    }
    Log("Fallback timer bridge installed; entity mutation stays disabled."); return true;
}
void GameThreadBridge::Uninstall(){
    if(!installed_.exchange(false)) return; gAdapter=nullptr;
    if(usingPluginSdk_){ PluginSdkBridge::Uninstall(); usingPluginSdk_=false; }
    if(timerQueue){ DeleteTimerQueueEx(timerQueue,INVALID_HANDLE_VALUE); timerQueue=nullptr; timerHandle=nullptr; }
    adapter_=nullptr;
}
