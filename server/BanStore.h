#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>
namespace gtanet {
struct Ban { std::string key,reason; uint64_t expiresMs=0; };
class BanStore {
    std::unordered_map<std::string,Ban> bans_;
public:
    void Add(std::string key,std::string reason,uint64_t expiresMs){bans_[key]={key,std::move(reason),expiresMs};}
    bool IsBanned(const std::string& key,uint64_t nowMs)const{auto i=bans_.find(key);return i!=bans_.end()&&(i->second.expiresMs==0||i->second.expiresMs>nowMs);}
    void Remove(const std::string& key){bans_.erase(key);}
};
}