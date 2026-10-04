#include "ResourceRuntime.h"
#include <algorithm>
namespace fs=std::filesystem;
namespace gtanet {
fs::file_time_type ResourceRuntime::NewestWrite(const fs::path& p)const{
 fs::file_time_type t{};std::error_code ec;
 for(auto it=fs::recursive_directory_iterator(p,ec);!ec&&it!=fs::recursive_directory_iterator();it.increment(ec))
   if(it->is_regular_file(ec)){auto x=it->last_write_time(ec);if(!ec&&x>t)t=x;}
 return t;
}
size_t ResourceRuntime::Discover(){
 std::error_code ec;if(!fs::exists(root_,ec))return 0;
 for(auto& e:fs::directory_iterator(root_,ec)){if(ec||!e.is_directory())continue;
   auto cfg=e.path()/"resource.cfg";if(!fs::exists(cfg))continue;auto mf=LoadManifest(cfg.string());if(mf.name.empty())mf.name=e.path().filename().string();
   RuntimeResource r;r.manifest=mf;r.directory=e.path();r.newestWrite=NewestWrite(e.path());items_[mf.name]=std::move(r);
 }return items_.size();
}
bool ResourceRuntime::DependenciesRunning(const RuntimeResource&r)const{
 for(auto&d:r.manifest.dependencies){auto i=items_.find(d);if(i==items_.end()||i->second.state!=ResourceState::Running)return false;}return true;
}
bool ResourceRuntime::Start(const std::string& name){
 auto i=items_.find(name);if(i==items_.end())return false;auto&r=i->second;if(r.state==ResourceState::Running)return true;
 if(!DependenciesRunning(r)){r.state=ResourceState::Failed;return false;}r.state=ResourceState::Starting;
 r.instance=std::make_unique<LuaResource>(r.manifest,r.directory.string(),api_);
 if(!r.instance->Start()){r.instance.reset();r.state=ResourceState::Failed;return false;}
 r.state=ResourceState::Running;r.newestWrite=NewestWrite(r.directory);return true;
}
bool ResourceRuntime::Stop(const std::string& name){
 auto i=items_.find(name);if(i==items_.end())return false;auto&r=i->second;if(r.instance){r.instance->Stop();r.instance.reset();}r.state=ResourceState::Stopped;return true;
}
bool ResourceRuntime::Reload(const std::string& name){if(items_.find(name)==items_.end())return false;Stop(name);return Start(name);}
void ResourceRuntime::Tick(uint64_t nowMs){for(auto&[n,r]:items_)if(r.state==ResourceState::Running&&r.instance)r.instance->Tick(nowMs);}
void ResourceRuntime::PollHotReload(){for(auto&[n,r]:items_)if(r.manifest.hotReload&&r.state==ResourceState::Running){auto t=NewestWrite(r.directory);if(t>r.newestWrite)Reload(n);}}
ResourceState ResourceRuntime::State(const std::string& n)const{auto i=items_.find(n);return i==items_.end()?ResourceState::Failed:i->second.state;}
std::vector<std::string> ResourceRuntime::Names()const{std::vector<std::string>v;for(auto&[n,r]:items_)v.push_back(n);std::sort(v.begin(),v.end());return v;}
}