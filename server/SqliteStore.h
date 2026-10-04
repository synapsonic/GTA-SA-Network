#pragma once
#include <string>
#include <cstdint>
#include "AccountStore.h"
#if defined(GTANET_WITH_SQLITE)
struct sqlite3;
#endif
namespace gtanet {
class SqliteStore {
#if defined(GTANET_WITH_SQLITE)
 sqlite3* db_=nullptr;
#endif
public:
 ~SqliteStore();
 bool Open(const std::string& path);
 void Close();
 bool Migrate();
 bool FindAccount(const std::string& username,Account& out);
 bool UpsertAccount(const Account& a);
 bool AddBan(const std::string& key,const std::string& reason,uint64_t expiresMs);
 bool IsBanned(const std::string& key,uint64_t nowMs);
};
}