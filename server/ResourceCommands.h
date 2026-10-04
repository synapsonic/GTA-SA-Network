#pragma once
#include "ResourceRuntime.h"
#include "AccountStore.h"
#include <string>
namespace gtanet {
inline bool HandleResourceAdmin(ResourceRuntime&rt,Role role,const std::string&cmd,const std::string&name){
 if(uint8_t(role)<uint8_t(Role::Admin))return false;
 if(cmd=="start")return rt.Start(name);if(cmd=="stop")return rt.Stop(name);if(cmd=="reload")return rt.Reload(name);return false;
}
}