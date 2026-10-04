#pragma once
#include <cstdint>
#include <functional>
#include "LocalVehicleBridge.h"
#include "VehicleOwnership.h"
#include "../shared/Protocol.h"
namespace gtanet {
class VehicleSyncController {
    LocalVehicleBridge local_;
    VehicleOwnership* ownership_;
    LocalVehiclePresence previous_{};
    uint64_t lastStateMs_=0;
    uint32_t sequence_=0;
public:
    explicit VehicleSyncController(VehicleOwnership* o):ownership_(o){}
    template<class Send>
    void Tick(uint32_t playerId,uint64_t nowMs,Send&& send){
        auto cur=local_.ReadPresence();
        if(cur.inVehicle && (!previous_.inVehicle || previous_.vehicleId!=cur.vehicleId || previous_.seat!=cur.seat)){
            VehicleEnterPacket e{}; e.playerId=playerId; e.vehicleId=cur.vehicleId; e.seat=cur.seat; send(&e,sizeof(e));
            if(cur.seat==0){ VehicleClaimPacket c{}; c.playerId=playerId; c.vehicleId=cur.vehicleId; c.seat=0; send(&c,sizeof(c)); }
        }
        if(previous_.inVehicle && (!cur.inVehicle || cur.vehicleId!=previous_.vehicleId)){
            VehicleExitPacket e{}; e.playerId=playerId; e.vehicleId=previous_.vehicleId; send(&e,sizeof(e));
        }
        if(cur.inVehicle && cur.seat==0 && ownership_ && ownership_->IsOwner(cur.vehicleId,playerId) && nowMs-lastStateMs_>=50){
            VehicleStatePacket st{};
            if(local_.ReadOwnedState(playerId,cur.vehicleId,st)){ st.sequence=++sequence_; send(&st,sizeof(st)); lastStateMs_=nowMs; }
        }
        previous_=cur; local_.SetLast(cur);
    }
};
}