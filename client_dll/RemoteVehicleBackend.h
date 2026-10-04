#pragma once
#include <cstdint>
#include "../shared/Protocol.h"
namespace gtanet {
class RemoteVehicleBackend {
public:
    virtual ~RemoteVehicleBackend() = default;
    virtual void* Spawn(const VehicleSpawnPacket& s)=0;
    virtual void Apply(void* handle,const VehicleStatePacket& s)=0;
    virtual void Destroy(void* handle)=0;
};
RemoteVehicleBackend* CreateRemoteVehicleBackend();
}