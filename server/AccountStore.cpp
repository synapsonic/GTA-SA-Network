#include "AccountStore.h"
#include <fstream>
#include <sstream>
namespace gtanet {
bool AccountStore::Load(const std::string& path){std::ifstream f(path);if(!f)return false;std::string l;
 while(std::getline(f,l)){std::istringstream s(l);Account a;int role=0;if(std::getline(s,a.username,'|')){std::string id,sc,r;
 if(!std::getline(s,id,'|')||!std::getline(s,a.passwordHash,'|')||!std::getline(s,r,'|')||!std::getline(s,sc))continue;
 try{a.id=std::stoul(id);role=std::stoi(r);a.role=(Role)role;a.score=std::stoi(sc);accounts_[a.username]=a;}catch(...){}}}return true;}
bool AccountStore::Save(const std::string& path)const{std::ofstream f(path,std::ios::trunc);if(!f)return false;for(auto&[k,a]:accounts_)f<<a.username<<"|"<<a.id<<"|"<<a.passwordHash<<"|"<<int(a.role)<<"|"<<a.score<<"\n";return true;}
const Account* AccountStore::Find(const std::string& u)const{auto i=accounts_.find(u);return i==accounts_.end()?nullptr:&i->second;}
bool AccountStore::Upsert(Account a){if(a.username.empty()||a.passwordHash.empty())return false;accounts_[a.username]=std::move(a);return true;}
}