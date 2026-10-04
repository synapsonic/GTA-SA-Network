#pragma once
#include <unordered_map>
#include <mutex>
#include "../shared/Protocol.h"
#include "RemoteVehicleBackend.h"
#include "VehicleSnapshotBuffer.h"
namespace gtanet {
class RemoteVehicleManager {
    struct Item { VehicleSpawnPacket spawn{}; VehicleStatePacket state{}; VehicleSnapshotBuffer snapshots{}; void* handle=nullptr; bool hasState=false; };
    std::unordered_map<uint32_t,Item> vehicles_;
    std::mutex mutex_;
    RemoteVehicleBackend* backend_=nullptr;
public:
    RemoteVehicleManager();
    ~RemoteVehicleManager();
    void OnSpawn(const VehicleSpawnPacket&);
    void OnState(const VehicleStatePacket&);
    void OnDestroy(uint32_t id);
    void Tick();
};
}