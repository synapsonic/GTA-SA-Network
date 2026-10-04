#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
namespace gtanet {
enum class Role:uint8_t{Player=0,Moderator=1,Admin=2,Owner=3};
struct Account { uint32_t id=0; std::string username,passwordHash; Role role=Role::Player; int32_t score=0; };
class AccountStore {
    std::unordered_map<std::string,Account> accounts_;
public:
    bool Load(const std::string& path);
    bool Save(const std::string& path) const;
    const Account* Find(const std::string& username) const;
    bool Upsert(Account a);
};
}