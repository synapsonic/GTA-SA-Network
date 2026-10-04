#include "GtaAdapter.h"
#ifdef GTANET_WITH_PLUGIN_SDK
#include "PluginSdkCompat.h"
#endif
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
namespace {
void Log(const std::string&s){std::ofstream("gtasa_network.log",std::ios::app)<<"[adapter] "<<s<<"\n";}
float Lerp(float a,float b,float t){return a+(b-a)*t;} float LerpAngle(float a,float b,float t){float d=std::fmod(b-a+540.f,360.f)-180.f;return a+d*t;}
bool Readable(const void*p,size_t n){MEMORY_BASIC_INFORMATION m{};if(!p||!VirtualQuery(p,&m,sizeof(m)))return false;if(m.State!=MEM_COMMIT||(m.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;return (uintptr_t)p+n<=(uintptr_t)m.BaseAddress+m.RegionSize;}
template<class T>bool SafeRead(uintptr_t a,T&o){auto*p=(const T*)a;if(!Readable(p,sizeof(T)))return false;o=*p;return true;}
constexpr uintptr_t PLAYER_PED_PTR=0xB6F5F0, MATRIX_PTR_OFF=0x14, MATRIX_POS_OFF=0x30, HEALTH_OFF=0x540, HEADING_OFF=0x558;
constexpr float STREAM_DISTANCE=180.0f, DESPAWN_DISTANCE=220.0f;
float Dist2(const gtanet::PlayerStatePacket&a,const gtanet::PlayerStatePacket&b){float x=a.x-b.x,y=a.y-b.y,z=a.z-b.z;return x*x+y*y+z*z;}
}
bool GtaAdapter::DetectSupportedBuild(){char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);std::string p(exe);std::transform(p.begin(),p.end(),p.begin(),[](unsigned char c){return(char)std::tolower(c);});if(p.find("gta_sa.exe")==std::string::npos)return false;uintptr_t ped=0;if(!SafeRead(PLAYER_PED_PTR,ped)){Log("Known 1.0 US player pointer is unreadable.");return false;}Log("GTA SA host detected; local-state adapter enabled.");return true;}
bool GtaAdapter::Initialize(){ready_=DetectSupportedBuild();
#ifdef GTANET_WITH_PLUGIN_SDK
entityApiEnabled_=true; Log("plugin-sdk CPed backend enabled.");
#else
entityApiEnabled_=false; Log("Standalone mode: remote CPed backend disabled.");
#endif
return ready_;}
bool GtaAdapter::ReadLocalPed(gtanet::PlayerStatePacket&o){uintptr_t ped=0,m=0;if(!SafeRead(PLAYER_PED_PTR,ped)||!ped||!SafeRead(ped+MATRIX_PTR_OFF,m)||!m)return false;struct V{float x,y,z;}v{};float hp=100,h=0;if(!SafeRead(m+MATRIX_POS_OFF,v))return false;SafeRead(ped+HEALTH_OFF,hp);SafeRead(ped+HEADING_OFF,h);if(!std::isfinite(v.x)||!std::isfinite(v.y)||!std::isfinite(v.z))return false;o.x=v.x;o.y=v.y;o.z=v.z;o.health=std::clamp(hp,0.f,1000.f);o.heading=h*57.295779513f;o.animation=0;o.animationFlags=0;o.weapon=0;o.weaponState=0;o.flags=0;o.movementState=0;return true;}
bool GtaAdapter::ReadLocalState(gtanet::PlayerStatePacket&o){return ready_&&ReadLocalPed(o);} bool GtaAdapter::CanUseGameEntityApi()const{return ready_&&entityApiEnabled_;}
void GtaAdapter::OnRemoteJoin(uint32_t id,const std::string& name,uint16_t skin){std::lock_guard<std::mutex>l(mutex_);auto& r=remotes_[id];r.name=name;r.skin=skin;} void GtaAdapter::OnRemoteLeave(uint32_t id){std::lock_guard<std::mutex>l(mutex_);auto i=remotes_.find(id);if(i!=remotes_.end()){DestroyRemotePed(i->second);remotes_.erase(i);}}
void GtaAdapter::PushRemoteState(const gtanet::PlayerStatePacket&s){std::lock_guard<std::mutex>l(mutex_);auto&r=remotes_[s.playerId];r.previous=r.hasState?r.target:s;r.target=s;r.alpha=0;r.hasState=true;}
void GtaAdapter::EnsureRemotePed(uint32_t id,Remote&r,const gtanet::PlayerStatePacket&v){if(r.ped||!CanUseGameEntityApi())return;(void)id;
#ifdef GTANET_WITH_PLUGIN_SDK
auto* ped=gtanet::sdk::SpawnPed(r.skin,v.x,v.y,v.z,v.heading,v.health); r.ped=reinterpret_cast<uintptr_t>(ped); r.streamed=ped!=nullptr;
#endif
}
void GtaAdapter::ApplyRemotePed(uint32_t id,Remote&r,const gtanet::PlayerStatePacket&v){if(!r.ped||!CanUseGameEntityApi())return;(void)id;
#ifdef GTANET_WITH_PLUGIN_SDK
gtanet::sdk::ApplyPed(reinterpret_cast<CPed*>(r.ped),v.x,v.y,v.z,v.heading,v.health);
// V0.9: animation/weapon fields are transported here; PluginSdkCompat owns the concrete mapping.
#endif
}
void GtaAdapter::DrawRemoteNameTag(uint32_t id,const Remote&r,const gtanet::PlayerStatePacket&v){(void)id;(void)r;(void)v;/* V0.9 nametag boundary: render name + health above the remote ped on the game thread. */}
void GtaAdapter::DestroyRemotePed(Remote&r){if(r.ped&&CanUseGameEntityApi()){
#ifdef GTANET_WITH_PLUGIN_SDK
gtanet::sdk::DestroyPed(reinterpret_cast<CPed*>(r.ped));
#endif
}r.ped=0;r.streamed=false;}
void GtaAdapter::TickRemotePlayers(float dt){gtanet::PlayerStatePacket local{};bool haveLocal=ReadLocalState(local);std::lock_guard<std::mutex>l(mutex_);for(auto&[id,r]:remotes_){if(!r.hasState)continue;r.alpha=std::min(1.f,r.alpha+dt*10.f);auto v=r.target;v.x=Lerp(r.previous.x,r.target.x,r.alpha);v.y=Lerp(r.previous.y,r.target.y,r.alpha);v.z=Lerp(r.previous.z,r.target.z,r.alpha);v.heading=LerpAngle(r.previous.heading,r.target.heading,r.alpha);float d2=haveLocal?Dist2(local,v):0.f;if(haveLocal&&d2>DESPAWN_DISTANCE*DESPAWN_DISTANCE){DestroyRemotePed(r);continue;}if(!haveLocal||d2<=STREAM_DISTANCE*STREAM_DISTANCE){EnsureRemotePed(id,r,v);ApplyRemotePed(id,r,v);DrawRemoteNameTag(id,r,v);}}}
