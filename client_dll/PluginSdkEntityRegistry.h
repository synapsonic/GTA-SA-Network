#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>
namespace gtanet {
class PluginSdkEntityRegistry {
 std::mutex m_;std::unordered_map<uint32_t,void*> peds_;
public:
 static PluginSdkEntityRegistry& Instance(){static PluginSdkEntityRegistry r;return r;}
 void BindPed(uint32_t id,void*p){std::lock_guard<std::mutex>l(m_);if(p)peds_[id]=p;}
 void UnbindPed(uint32_t id){std::lock_guard<std::mutex>l(m_);peds_.erase(id);}
 void* Ped(uint32_t id){std::lock_guard<std::mutex>l(m_);auto i=peds_.find(id);return i==peds_.end()?nullptr:i->second;}
#if defined(GTANET_WITH_PLUGIN_SDK) && defined(GTANET_PLUGINSDK_SEAT_API)
 bool AssignSeat(void* ped,void* vehicle,int8_t seat);
 void RemoveFromVehicle(void* ped,void* vehicle);
#endif
};
}