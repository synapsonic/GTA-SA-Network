#include "../shared/Protocol.h"
#include "../server/LagCompensation.h"
#include "../server/WeaponRules.h"
#include "../server/ChatService.h"
#include <cassert>
#include <iostream>
int main(){
 using namespace gtanet;
 static_assert(VERSION>=10);
 HelloPacket h{}; assert(h.h.magic==MAGIC); assert(h.h.version==VERSION);
 LagCompensation lag;lag.Record(1,1000,0,0,0);lag.Record(1,1100,10,0,0);Vec3 p{};
 assert(lag.Rewind(1,1050,p));assert(p.x>4.9f&&p.x<5.1f);
 assert(LagCompensation::ClampRewind(1000,100)<1000);
 assert(Weapon(34).range>=300.f);
 ChatService chat;assert(chat.Accept(1,1000,"hello"));assert(!chat.Accept(1,1100,"spam"));
 std::cout<<"protocol/server tests passed\n";return 0;
}