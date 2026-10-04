#include "RemoteVehicleManager.h"
#include "VehicleIdRegistry.h"
#include <chrono>
namespace gtanet {
RemoteVehicleManager::RemoteVehicleManager():backend_(CreateRemoteVehicleBackend()){}
RemoteVehicleManager::~RemoteVehicleManager(){ delete backend_; }
void RemoteVehicleManager::OnSpawn(const VehicleSpawnPacket& s){ std::lock_guard<std::mutex> l(mutex_); vehicles_[s.vehicleId].spawn=s; }
void RemoteVehicleManager::OnState(const VehicleStatePacket& s){ std::lock_guard<std::mutex> l(mutex_); auto &v=vehicles_[s.vehicleId]; v.state=s; v.snapshots.Push(s); v.hasState=true; }
void RemoteVehicleManager::OnDestroy(uint32_t id){ std::lock_guard<std::mutex> l(mutex_); auto it=vehicles_.find(id); if(it!=vehicles_.end()){ if(it->second.handle&&backend_) backend_->Destroy(it->second.handle); vehicles_.erase(it); } }
void RemoteVehicleManager::Tick(){ std::lock_guard<std::mutex> l(mutex_); if(!backend_)return; for(auto &[id,v]:vehicles_){ if(!v.handle) v.handle=backend_->Spawn(v.spawn); if(v.handle&&v.hasState) {
            using namespace std::chrono;
            uint64_t now=(uint64_t)duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
            VehicleStatePacket sampled{};
            uint64_t renderTime=now>100?now-100:0;
            if(v.snapshots.Sample(renderTime,sampled)) backend_->Apply(v.handle,sampled);
        } } }
}