#pragma once
#include <cstdint>
namespace gtanet {
class RemoteSeatAdapter {
public:
    // Must be invoked on GTA's game thread.
    bool PutRemotePedInVehicle(uint32_t playerId,uint32_t vehicleId,int8_t seat);
    void RemoveRemotePedFromVehicle(uint32_t playerId,uint32_t vehicleId);
};
}