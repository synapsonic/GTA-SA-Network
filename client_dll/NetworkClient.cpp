#include "NetworkClient.h"
#include "GtaAdapter.h"
#include "../shared/Protocol.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>
#include <chrono>
#include <cstring>
#include <fstream>
#include <mutex>
#include <thread>
#include <vector>
#pragma comment(lib,"ws2_32.lib")
using namespace gtanet;
namespace {
std::mutex logMutex; std::atomic<bool> running{false}; SOCKET activeSocket=INVALID_SOCKET; GtaAdapter adapter;
void Log(const std::string&s){std::lock_guard<std::mutex>l(logMutex);std::ofstream("gtasa_network.log",std::ios::app)<<"[net] "<<s<<"\n";}
bool recvAll(SOCKET s,char*p,int n){while(n){int r=recv(s,p,n,0);if(r<=0)return false;p+=r;n-=r;}return true;}
bool sendAll(SOCKET s,const char*p,int n){while(n){int r=send(s,p,n,0);if(r<=0)return false;p+=r;n-=r;}return true;}
}
void StopNetworkClient(){running=false;if(activeSocket!=INVALID_SOCKET)shutdown(activeSocket,SD_BOTH);}
void StartNetworkClient(const std::string& host,unsigned short port,const std::string& playerName){
 if(running.exchange(true))return;
 std::thread([=]{WSADATA w{};if(WSAStartup(MAKEWORD(2,2),&w)){running=false;return;} adapter.Initialize();
 addrinfo hints{};hints.ai_family=AF_INET;hints.ai_socktype=SOCK_STREAM;addrinfo*r=nullptr;if(getaddrinfo(host.c_str(),std::to_string(port).c_str(),&hints,&r)){Log("DNS error");WSACleanup();running=false;return;}
 SOCKET s=socket(r->ai_family,r->ai_socktype,r->ai_protocol);activeSocket=s;if(s==INVALID_SOCKET||connect(s,r->ai_addr,(int)r->ai_addrlen)){Log("Connection failed");if(s!=INVALID_SOCKET)closesocket(s);freeaddrinfo(r);WSACleanup();running=false;return;}freeaddrinfo(r);
 HelloPacket hello{};strncpy_s(hello.name,playerName.c_str(),NAME_LEN-1);hello.skin=0;if(!sendAll(s,(char*)&hello,sizeof(hello))){closesocket(s);WSACleanup();running=false;return;}Log("Connected to "+host+":"+std::to_string(port));
 std::thread sender([&]{uint32_t seq=0;auto lastPing=std::chrono::steady_clock::now();while(running){PlayerStatePacket st{};if(adapter.ReadLocalState(st)){st.sequence=++seq;if(!sendAll(s,(char*)&st,sizeof(st)))break;}auto now=std::chrono::steady_clock::now();if(now-lastPing>std::chrono::seconds(3)){PingPacket p{};p.tick=(uint64_t)std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();if(!sendAll(s,(char*)&p,sizeof(p)))break;lastPing=now;}std::this_thread::sleep_for(std::chrono::milliseconds(50));}running=false;shutdown(s,SD_BOTH);});
 while(running){PacketHeader h{};if(!recvAll(s,(char*)&h,sizeof(h))||!ValidHeader(h))break;std::vector<char>b(h.size);memcpy(b.data(),&h,sizeof(h));if(h.size>sizeof(h)&&!recvAll(s,b.data()+sizeof(h),h.size-sizeof(h)))break;switch(h.type){case PacketType::Welcome:Log("Assigned player id "+std::to_string(((WelcomePacket*)b.data())->playerId));break;case PacketType::PlayerJoin:{auto*p=(PlayerJoinPacket*)b.data();adapter.OnRemoteJoin(p->playerId,std::string(p->name,strnlen(p->name,NAME_LEN)),p->skin);Log("Player joined #"+std::to_string(p->playerId));break;}case PacketType::PlayerLeave:{auto id=((PlayerLeavePacket*)b.data())->playerId;adapter.OnRemoteLeave(id);Log("Player left #"+std::to_string(id));break;}case PacketType::PlayerState:if(h.size==sizeof(PlayerStatePacket))adapter.PushRemoteState(*(PlayerStatePacket*)b.data());break;case PacketType::Pong:break;default:break;}}
 running=false;shutdown(s,SD_BOTH);if(sender.joinable())sender.join();closesocket(s);activeSocket=INVALID_SOCKET;WSACleanup();Log("Disconnected");}).detach();
}
