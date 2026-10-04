#include "GameMode.h"
namespace gtanet {
class FreeroamGameMode final:public GameMode{
public:
 const char* Name()const override{return "freeroam";}
 void OnPlayerConnect(uint32_t)override{}
 void OnPlayerDeath(uint32_t,uint32_t,uint8_t)override{}
 bool OnCommand(uint32_t,const std::string& cmd)override{
   if(cmd=="/help"||cmd=="/spawn"||cmd=="/kill")return true;
   return false;
 }
};
}