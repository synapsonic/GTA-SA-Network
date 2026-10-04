#pragma once
#include <atomic>
class GtaAdapter;
class GameThreadBridge {
public:
    bool Install(GtaAdapter* adapter); void Uninstall();
    bool Installed() const { return installed_.load(); }
private:
    std::atomic<bool> installed_{false}; GtaAdapter* adapter_=nullptr; bool usingPluginSdk_=false;
};
