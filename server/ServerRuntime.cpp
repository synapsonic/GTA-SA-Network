#include "ServerRuntime.h"
namespace gtanet {
bool ServerRuntime::Start(){
 if(running_)return true;log_.Info("Starting "+config_.name);
#if defined(GTANET_WITH_SQLITE)
 if(!db_.Open("server.db")||!db_.Migrate()){log_.Error("SQLite initialization failed");return false;}
#else
 log_.Warn("SQLite disabled; persistent SQL account/ban storage unavailable");
#endif
 ScriptApi api{};
 // Bind these callbacks to the network/player/vehicle registries in server/main.cpp.
 api.broadcast=[this](const std::string&m){log_.Info("[broadcast] "+m);};
 api.messagePlayer=[this](uint32_t p,const std::string&m){log_.Info("[player "+std::to_string(p)+"] "+m);};
 resources_=std::make_unique<ResourceRuntime>("resources",api);
 auto count=resources_->Discover();log_.Info("Discovered "+std::to_string(count)+" resources");
 running_=true;return true;
}
void ServerRuntime::Tick(uint64_t now){if(resources_)resources_->Tick(now);}
void ServerRuntime::Stop(){if(!running_)return;log_.Info("Stopping server");resources_.reset();db_.Close();running_=false;}
}