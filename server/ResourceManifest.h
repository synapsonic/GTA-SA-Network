#pragma once
#include "LuaResource.h"
#include <fstream>
#include <sstream>
#include <algorithm>
namespace gtanet {
inline std::string RTrim(std::string s){while(!s.empty()&&(s.back()=='\r'||s.back()=='\n'||s.back()==' '||s.back()=='\t'))s.pop_back();auto p=s.find_first_not_of(" \t");return p==std::string::npos?"":s.substr(p);}
inline ResourceManifest LoadManifest(const std::string& path){
 ResourceManifest m;std::ifstream f(path);std::string l;
 while(std::getline(f,l)){l=RTrim(l);if(l.empty()||l[0]=='#')continue;auto p=l.find('=');if(p==std::string::npos)continue;
 auto k=RTrim(l.substr(0,p)),v=RTrim(l.substr(p+1));
 if(k=="name")m.name=v;else if(k=="entry")m.entry=v;else if(k=="hot_reload")m.hotReload=(v=="true"||v=="1");
 else if(k=="depends"){std::stringstream ss(v);std::string d;while(std::getline(ss,d,',')){d=RTrim(d);if(!d.empty())m.dependencies.push_back(d);}}
 }return m;
}
}