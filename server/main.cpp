#include "SocketCompat.h"
#include "ServerConfig.h"
#include "ServerRuntime.h"
#include "ChatService.h"
#include "VehicleAuthority.h"
#include "CombatAuthority.h"
#include "LagCompensation.h"
#include "RespawnManager.h"
#include "../shared/Protocol.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstring>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
using namespace gtanet;
struct Client{SocketHandle s=INVALID_NET_SOCKET;uint32_t id=0;std::string name;uint16_t skin=0;uint64_t connectedMs=0;};
static std::mutex gMutex;static std::unordered_map<uint32_t,Client>gClients;static std::atomic<uint32_t>gNext{1};static std::atomic<bool>gRun{true};
static ChatService gChat;static VehicleAuthority gVehicles;static CombatAuthority gCombat;static LagCompensation gLag;static RespawnManager gRespawns;
static uint64_t NowMs(){using namespace std::chrono;return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();}
static bool RecvAll(SocketHandle s,char*p,size_t n){while(n){auto r=recv(s,p,(int)n,0);if(r<=0)return false;p+=r;n-=size_t(r);}return true;}
static bool SendAll(SocketHandle s,const char*p,size_t n){while(n){auto r=send(s,p,(int)n,0);if(r<=0)return false;p+=r;n-=size_t(r);}return true;}
static void Broadcast(const void*p,size_t n,uint32_t except=0){std::lock_guard<std::mutex>l(gMutex);for(auto&[id,c]:gClients)if(id!=except)SendAll(c.s,(const char*)p,n);}
static void System(const std::string&t){SystemMessagePacket p{};std::strncpy(p.text,t.c_str(),CHAT_LEN-1);Broadcast(&p,sizeof(p));}
static void ClientThread(SocketHandle s,const ServerConfig cfg){
 PacketHeader h{};if(!RecvAll(s,(char*)&h,sizeof(h))||!ValidHeader(h)||h.type!=PacketType::Hello||h.size!=sizeof(HelloPacket)){CloseSocket(s);return;}
 HelloPacket hello{};hello.h=h;if(!RecvAll(s,((char*)&hello)+sizeof(h),sizeof(hello)-sizeof(h))){CloseSocket(s);return;}
 uint32_t id=gNext++;std::string name(hello.name,strnlen(hello.name,NAME_LEN));if(name.empty())name="Player"+std::to_string(id);if(name.size()>NAME_LEN-1)name.resize(NAME_LEN-1);
 {std::lock_guard<std::mutex>l(gMutex);if(gClients.size()>=cfg.maxPlayers){CloseSocket(s);return;}for(auto&[oid,c]:gClients){PlayerJoinPacket old{};old.playerId=oid;std::strncpy(old.name,c.name.c_str(),NAME_LEN-1);old.skin=c.skin;SendAll(s,(char*)&old,sizeof(old));}gClients[id]={s,id,name,hello.skin,NowMs()};}
 WelcomePacket w{};w.playerId=id;SendAll(s,(char*)&w,sizeof(w));ServerInfoPacket info{};std::strncpy(info.name,cfg.name.c_str(),SERVER_NAME_LEN-1);info.maxPlayers=cfg.maxPlayers;{std::lock_guard<std::mutex>l(gMutex);info.onlinePlayers=(uint16_t)gClients.size();}info.passworded=cfg.password.empty()?0:1;SendAll(s,(char*)&info,sizeof(info));
 PlayerJoinPacket j{};j.playerId=id;std::strncpy(j.name,name.c_str(),NAME_LEN-1);j.skin=hello.skin;Broadcast(&j,sizeof(j),id);if(cfg.announceJoinLeave)System(name+" joined the server");
 while(gRun){PacketHeader ph{};if(!RecvAll(s,(char*)&ph,sizeof(ph))||!ValidHeader(ph))break;std::vector<char>b(ph.size);std::memcpy(b.data(),&ph,sizeof(ph));if(ph.size>sizeof(ph)&&!RecvAll(s,b.data()+sizeof(ph),ph.size-sizeof(ph)))break;
  if(ph.type==PacketType::PlayerState&&ph.size==sizeof(PlayerStatePacket)){auto*st=(PlayerStatePacket*)b.data();st->playerId=id;st->health=std::max(0.f,std::min(100.f,st->health));gCombat.Player(id).x=st->x;gCombat.Player(id).y=st->y;gCombat.Player(id).z=st->z;gLag.Record(id,NowMs(),st->x,st->y,st->z);Broadcast(st,sizeof(*st),id);}
  else if(ph.type==PacketType::VehicleClaim&&ph.size==sizeof(VehicleClaimPacket)){auto*c=(VehicleClaimPacket*)b.data();VehicleAuthorityState a{};if(gVehicles.Claim(c->vehicleId,id,c->seat,a)){VehicleOwnershipPacket o{};o.vehicleId=c->vehicleId;o.ownerPlayerId=a.ownerPlayerId;o.revision=a.revision;Broadcast(&o,sizeof(o));}}
  else if(ph.type==PacketType::VehicleState&&ph.size==sizeof(VehicleStatePacket)){auto*st=(VehicleStatePacket*)b.data();if(gVehicles.CanPublish(st->vehicleId,id)){st->ownerPlayerId=id;st->serverTimeMs=NowMs();Broadcast(st,sizeof(*st),id);}}
  else if(ph.type==PacketType::VehicleEnter&&ph.size==sizeof(VehicleEnterPacket)){auto*e=(VehicleEnterPacket*)b.data();e->playerId=id;Broadcast(e,sizeof(*e),id);}
  else if(ph.type==PacketType::VehicleExit&&ph.size==sizeof(VehicleExitPacket)){auto*e=(VehicleExitPacket*)b.data();e->playerId=id;VehicleAuthorityState a{};gVehicles.Exit(e->vehicleId,id,a);Broadcast(e,sizeof(*e),id);VehicleOwnershipPacket o{};o.vehicleId=e->vehicleId;o.ownerPlayerId=a.ownerPlayerId;o.revision=a.revision;Broadcast(&o,sizeof(o));}
  else if(ph.type==PacketType::AimState&&ph.size==sizeof(AimStatePacket)){auto*p=(AimStatePacket*)b.data();p->playerId=id;Broadcast(p,sizeof(*p),id);}
  else if(ph.type==PacketType::Shot&&ph.size==sizeof(ShotPacket)){auto*p=(ShotPacket*)b.data();p->playerId=id;if(gCombat.RegisterShot(id,id,p->weapon,NowMs()))Broadcast(p,sizeof(*p),id);}
  else if(ph.type==PacketType::ChatMessage&&ph.size==sizeof(ChatMessagePacket)){auto*p=(ChatMessagePacket*)b.data();p->playerId=id;std::string text(p->text,strnlen(p->text,CHAT_LEN));if(gChat.Accept(id,NowMs(),text))Broadcast(p,sizeof(*p));}
  else if(ph.type==PacketType::ScoreboardRequest&&ph.size==sizeof(ScoreboardRequestPacket)){std::lock_guard<std::mutex>l(gMutex);for(auto&[pid,c]:gClients){ScoreboardEntryPacket e{};e.playerId=pid;std::strncpy(e.name,c.name.c_str(),NAME_LEN-1);SendAll(s,(char*)&e,sizeof(e));}}
  else if(ph.type==PacketType::Ping&&ph.size==sizeof(PingPacket)){PongPacket p{};p.tick=((PingPacket*)b.data())->tick;SendAll(s,(char*)&p,sizeof(p));}
 }
 {std::lock_guard<std::mutex>l(gMutex);gClients.erase(id);}gChat.Remove(id);gVehicles.Disconnect(id);gCombat.Remove(id);gLag.Remove(id);gRespawns.Remove(id);CloseSocket(s);PlayerLeavePacket q{};q.playerId=id;Broadcast(&q,sizeof(q));if(cfg.announceJoinLeave)System(name+" left the server");
}
static void Signal(int){gRun=false;}
int main(int argc,char**argv){std::signal(SIGINT,Signal);std::signal(SIGTERM,Signal);ServerConfig cfg=ServerConfig::Load(argc>1?argv[1]:"config/server.cfg");if(argc>2)cfg.port=(uint16_t)std::stoi(argv[2]);ServerRuntime runtime(cfg);if(!runtime.Start()||!NetInit())return 1;
 SocketHandle ls=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);if(ls==INVALID_NET_SOCKET)return 2;int yes=1;setsockopt(ls,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(yes));sockaddr_in a{};a.sin_family=AF_INET;a.sin_addr.s_addr=INADDR_ANY;a.sin_port=htons(cfg.port);if(bind(ls,(sockaddr*)&a,sizeof(a))||listen(ls,SOMAXCONN)){std::cerr<<"Cannot listen on "<<cfg.port<<"\n";return 3;}std::cout<<cfg.name<<" listening on "<<cfg.port<<"\n";
 std::thread tick([&]{uint64_t lastHot=0;while(gRun){auto n=NowMs();runtime.Tick(n);if(n-lastHot>1000){lastHot=n;}std::this_thread::sleep_for(std::chrono::milliseconds(10));}});
 while(gRun){sockaddr_in peer{};
#ifdef _WIN32
 int plen=sizeof(peer);
#else
 socklen_t plen=sizeof(peer);
#endif
 SocketHandle s=accept(ls,(sockaddr*)&peer,&plen);if(s!=INVALID_NET_SOCKET)std::thread(ClientThread,s,cfg).detach();else if(!gRun)break;}
 CloseSocket(ls);runtime.Stop();NetCleanup();if(tick.joinable())tick.join();return 0;}
