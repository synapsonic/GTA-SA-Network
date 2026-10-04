#pragma once
#include <cstdint>
#include "../shared/Protocol.h"
namespace gtanet {
struct LocalVehiclePresence { bool inVehicle=false; uint32_t vehicleId=0; int8_t seat=-1; };
class LocalVehicleBridge {
    LocalVehiclePresence last_{};
public:
    LocalVehiclePresence ReadPresence();
    bool ReadOwnedState(uint32_t localPlayerId,uint32_t vehicleId,VehicleStatePacket& out);
    const LocalVehiclePresence& Last() const { return last_; }
    void SetLast(const LocalVehiclePresence& p){ last_=p; }
};
}