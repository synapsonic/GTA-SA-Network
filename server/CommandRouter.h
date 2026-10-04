#pragma once
#include <string>
#include <sstream>
#include <vector>
namespace gtanet {
struct Command { std::string name; std::vector<std::string> args; };
inline Command ParseCommand(std::string text){
    Command c; if(!text.empty()&&text[0]=='/')text.erase(0,1);
    std::istringstream ss(text); ss>>c.name; std::string a; while(ss>>a)c.args.push_back(a); return c;
}
// Suggested built-ins: /help, /players, /ping, /kill, /respawn.
// Privileged commands should be authenticated separately; never infer admin rights from nickname.
}