#include "ServerConfig.h"
#include <fstream>
#include <algorithm>
namespace gtanet {
static std::string trim(std::string s){while(!s.empty()&&(s.back()=='\r'||s.back()=='\n'||s.back()==' '))s.pop_back();auto p=s.find_first_not_of(" \t");return p==std::string::npos?"":s.substr(p);}
ServerConfig ServerConfig::Load(const std::string& path){
    ServerConfig c; std::ifstream f(path); std::string line;
    while(std::getline(f,line)){ line=trim(line); if(line.empty()||line[0]=='#')continue; auto e=line.find('=');if(e==std::string::npos)continue;
        auto k=trim(line.substr(0,e)),v=trim(line.substr(e+1));
        if(k=="name")c.name=v; else if(k=="port")c.port=(uint16_t)std::clamp(std::stoi(v),1,65535);
        else if(k=="max_players")c.maxPlayers=(uint16_t)std::clamp(std::stoi(v),1,500);
        else if(k=="password")c.password=v; else if(k=="welcome")c.welcome=v;
        else if(k=="announce_join_leave")c.announceJoinLeave=(v=="1"||v=="true"||v=="yes");
    } return c;
}
}