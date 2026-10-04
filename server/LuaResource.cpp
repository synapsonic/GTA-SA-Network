#include "LuaResource.h"
#if defined(GTANET_WITH_LUA)
#include <lua.hpp>
#endif
namespace gtanet {
LuaResource::~LuaResource(){Stop();}
#if defined(GTANET_WITH_LUA)
static ScriptApi* api(lua_State*L){return static_cast<ScriptApi*>(lua_touserdata(L,lua_upvalueindex(1)));}
static int l_broadcast(lua_State*L){auto*a=api(L);const char*s=luaL_checkstring(L,1);if(a&&a->broadcast)a->broadcast(s);return 0;}
static int l_message(lua_State*L){auto*a=api(L);auto p=(uint32_t)luaL_checkinteger(L,1);const char*s=luaL_checkstring(L,2);if(a&&a->messagePlayer)a->messagePlayer(p,s);return 0;}
static int l_setpos(lua_State*L){auto*a=api(L);uint32_t p=(uint32_t)luaL_checkinteger(L,1);float x=luaL_checknumber(L,2),y=luaL_checknumber(L,3),z=luaL_checknumber(L,4),h=luaL_optnumber(L,5,0);if(a&&a->setPlayerPos)a->setPlayerPos(p,x,y,z,h);return 0;}
static int l_sethealth(lua_State*L){auto*a=api(L);uint32_t p=(uint32_t)luaL_checkinteger(L,1);float h=luaL_checknumber(L,2);if(a&&a->setPlayerHealth)a->setPlayerHealth(p,h);return 0;}
static int l_createvehicle(lua_State*L){auto*a=api(L);uint16_t m=(uint16_t)luaL_checkinteger(L,1);float x=luaL_checknumber(L,2),y=luaL_checknumber(L,3),z=luaL_checknumber(L,4),h=luaL_optnumber(L,5,0);uint32_t id=a&&a->createVehicle?a->createVehicle(m,x,y,z,h):0;lua_pushinteger(L,id);return 1;}
static int l_destroyvehicle(lua_State*L){auto*a=api(L);uint32_t id=(uint32_t)luaL_checkinteger(L,1);if(a&&a->destroyVehicle)a->destroyVehicle(id);return 0;}
static void bind(lua_State*L,ScriptApi*a){
 lua_newtable(L);
 lua_pushlightuserdata(L,a);lua_pushcclosure(L,l_broadcast,1);lua_setfield(L,-2,"broadcast");
 lua_pushlightuserdata(L,a);lua_pushcclosure(L,l_message,1);lua_setfield(L,-2,"message_player");
 lua_setglobal(L,"server");
 lua_newtable(L);
 lua_pushlightuserdata(L,a);lua_pushcclosure(L,l_setpos,1);lua_setfield(L,-2,"set_pos");
 lua_pushlightuserdata(L,a);lua_pushcclosure(L,l_sethealth,1);lua_setfield(L,-2,"set_health");
 lua_setglobal(L,"player");
 lua_newtable(L);
 lua_pushlightuserdata(L,a);lua_pushcclosure(L,l_createvehicle,1);lua_setfield(L,-2,"create");
 lua_pushlightuserdata(L,a);lua_pushcclosure(L,l_destroyvehicle,1);lua_setfield(L,-2,"destroy");
 lua_setglobal(L,"vehicle");
}
#endif
bool LuaResource::Start(){
#if defined(GTANET_WITH_LUA)
 Stop();L_=luaL_newstate();if(!L_)return false;
 // Sandbox: only selected standard libraries. No io/os/package/debug by default.
 luaL_requiref(L_,"_G",luaopen_base,1);lua_pop(L_,1);
 luaL_requiref(L_,"math",luaopen_math,1);lua_pop(L_,1);
 luaL_requiref(L_,"string",luaopen_string,1);lua_pop(L_,1);
 luaL_requiref(L_,"table",luaopen_table,1);lua_pop(L_,1);
 bind(L_,&api_);
 std::string path=directory_+"/"+manifest_.entry;
 if(luaL_loadfile(L_,path.c_str())||lua_pcall(L_,0,0,0)){Stop();return false;}
 Event("on_start");return true;
#else
 return false;
#endif
}
void LuaResource::Stop(){
#if defined(GTANET_WITH_LUA)
 if(L_){Event("on_stop");lua_close(L_);L_=nullptr;}
#endif
}
void LuaResource::Tick(uint64_t nowMs){
#if defined(GTANET_WITH_LUA)
 if(!L_)return;lua_getglobal(L_,"on_tick");if(!lua_isfunction(L_,-1)){lua_pop(L_,1);return;}lua_pushinteger(L_,nowMs);if(lua_pcall(L_,1,0,0))lua_pop(L_,1);
#else
 (void)nowMs;
#endif
}
bool LuaResource::Event(const char*name,uint32_t a,uint32_t b,uint32_t c){
#if defined(GTANET_WITH_LUA)
 if(!L_)return false;lua_getglobal(L_,name);if(!lua_isfunction(L_,-1)){lua_pop(L_,1);return false;}
 lua_pushinteger(L_,a);lua_pushinteger(L_,b);lua_pushinteger(L_,c);if(lua_pcall(L_,3,0,0)){lua_pop(L_,1);return false;}return true;
#else
 (void)name;(void)a;(void)b;(void)c;return false;
#endif
}
bool LuaResource::Command(uint32_t player,const std::string&command){
#if defined(GTANET_WITH_LUA)
 if(!L_)return false;lua_getglobal(L_,"on_command");if(!lua_isfunction(L_,-1)){lua_pop(L_,1);return false;}
 lua_pushinteger(L_,player);lua_pushlstring(L_,command.data(),command.size());if(lua_pcall(L_,2,1,0)){lua_pop(L_,1);return false;}
 bool handled=lua_toboolean(L_,-1);lua_pop(L_,1);return handled;
#else
 (void)player;(void)command;return false;
#endif
}
}