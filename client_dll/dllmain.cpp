#include "ClientLog.h"
#include "ClientCapabilities.h"
#include <windows.h>
#include <string>
#include "NetworkClient.h"
static std::string V(const char*k,const char*d){char b[512];GetPrivateProfileStringA("network",k,d,b,sizeof(b),".\\network.ini");return b;}
static DWORD WINAPI Init(LPVOID){Sleep(2500);try{auto host=V("host","127.0.0.1");auto port=(unsigned short)std::stoi(V("port","7777"));auto name=V("name","Player");StartNetworkClient(host,port,name);}catch(...){ }return 0;}
BOOL APIENTRY DllMain(HMODULE h,DWORD reason,LPVOID){if(reason==DLL_PROCESS_ATTACH){DisableThreadLibraryCalls(h);CreateThread(nullptr,0,Init,nullptr,0,nullptr);}else if(reason==DLL_PROCESS_DETACH){StopNetworkClient();}return TRUE;}
