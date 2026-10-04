#include "LocalVehicleBridge.h"
#include "VehicleIdRegistry.h"
#if defined(GTANET_WITH_PLUGIN_SDK)
#include <plugin.h>
#include <game_sa/CPlayerPed.h>
#include <game_sa/CVehicle.h>
#endif
namespace gtanet {
LocalVehiclePresence LocalVehicleBridge::ReadPresence(){
 LocalVehiclePresence p{};
#if defined(GTANET_WITH_PLUGIN_SDK)
 auto* ped=FindPlayerPed();
 if(ped && ped->m_pVehicle){
   auto* v=ped->m_pVehicle;uint32_t id=VehicleIdRegistry::Instance().Id(v);
   if(id){p.inVehicle=true;p.vehicleId=id;p.seat=(v->m_pDriver==ped)?0:-1;
     if(p.seat<0)for(int i=0;i<8;i++)if(v->m_apPassengers[i]==ped){p.seat=(int8_t)(i+1);break;}
   }
 }
#endif
 last_=p;return p;
}
bool LocalVehicleBridge::ReadOwnedState(uint32_t localPlayerId,uint32_t vehicleId,VehicleStatePacket& out){
 (void)localPlayerId;
#if defined(GTANET_WITH_PLUGIN_SDK)
 auto*v=static_cast<CVehicle*>(VehicleIdRegistry::Instance().Handle(vehicleId));if(!v)return false;
 auto pos=v->GetPosition();out.vehicleId=vehicleId;out.x=pos.x;out.y=pos.y;out.z=pos.z;
 out.heading=v->GetHeading();out.vx=v->m_vecMoveSpeed.x;out.vy=v->m_vecMoveSpeed.y;out.vz=v->m_vecMoveSpeed.z;out.health=v->m_fHealth;return true;
#else
 (void)vehicleId;(void)out;return false;
#endif
}
}