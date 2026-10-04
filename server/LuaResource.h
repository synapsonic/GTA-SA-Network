#pragma once
#include "Resource.h"
#include "ScriptApi.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdint>

#if defined(GTANET_WITH_LUA)
struct lua_State;
#endif

namespace gtanet {
struct ResourceManifest {
    std::string name;
    std::string entry="main.lua";
    bool hotReload=true;
    std::vector<std::string> dependencies;
};
class LuaResource final : public Resource {
    ResourceManifest manifest_;
    std::string directory_;
    ScriptApi api_;
#if defined(GTANET_WITH_LUA)
    lua_State* L_=nullptr;
#endif
public:
    LuaResource(ResourceManifest m,std::string dir,ScriptApi api)
      :manifest_(std::move(m)),directory_(std::move(dir)),api_(std::move(api)){}
    ~LuaResource() override;
    const char* Name()const override{return manifest_.name.c_str();}
    bool Start() override;
    void Stop() override;
    void Tick(uint64_t nowMs) override;
    bool Event(const char* name,uint32_t a=0,uint32_t b=0,uint32_t c=0);
    bool Command(uint32_t player,const std::string& command);
};
}