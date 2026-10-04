#pragma once
#include "AccountStore.h"
#include <string>
#include <unordered_map>
#include <cstdint>
namespace gtanet {
struct Session { uint32_t playerId=0,accountId=0; Role role=Role::Player; std::string username; };
class AuthService {
    const AccountStore* store_;
    std::unordered_map<uint32_t,Session> sessions_;
public:
    explicit AuthService(const AccountStore* s):store_(s){}
    // proof must already be a password KDF result/derived proof; never send/store plaintext passwords.
    bool Authenticate(uint32_t playerId,const std::string& user,const std::string& proof,Session& out){
        auto*a=store_?store_->Find(user):nullptr;if(!a||a->passwordHash!=proof)return false;
        out={playerId,a->id,a->role,a->username};sessions_[playerId]=out;return true;
    }
    const Session* Get(uint32_t p)const{auto i=sessions_.find(p);return i==sessions_.end()?nullptr:&i->second;}
    bool AtLeast(uint32_t p,Role r)const{auto*s=Get(p);return s&&uint8_t(s->role)>=uint8_t(r);}
    void Remove(uint32_t p){sessions_.erase(p);}
};
}