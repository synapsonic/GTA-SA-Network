#include "SqliteStore.h"
#if defined(GTANET_WITH_SQLITE)
#include <sqlite3.h>
#endif
namespace gtanet {
SqliteStore::~SqliteStore(){Close();}
bool SqliteStore::Open(const std::string& path){
#if defined(GTANET_WITH_SQLITE)
 return sqlite3_open(path.c_str(),&db_)==SQLITE_OK;
#else
 (void)path;return false;
#endif
}
void SqliteStore::Close(){
#if defined(GTANET_WITH_SQLITE)
 if(db_){sqlite3_close(db_);db_=nullptr;}
#endif
}
bool SqliteStore::Migrate(){
#if defined(GTANET_WITH_SQLITE)
 const char* sql=
 "PRAGMA journal_mode=WAL;"
 "CREATE TABLE IF NOT EXISTS accounts(id INTEGER PRIMARY KEY,username TEXT UNIQUE NOT NULL,password_hash TEXT NOT NULL,role INTEGER NOT NULL DEFAULT 0,score INTEGER NOT NULL DEFAULT 0);"
 "CREATE TABLE IF NOT EXISTS bans(key TEXT PRIMARY KEY,reason TEXT NOT NULL,expires_ms INTEGER NOT NULL DEFAULT 0);";
 return db_&&sqlite3_exec(db_,sql,nullptr,nullptr,nullptr)==SQLITE_OK;
#else
 return false;
#endif
}
bool SqliteStore::FindAccount(const std::string& u,Account& out){
#if defined(GTANET_WITH_SQLITE)
 if(!db_)return false;sqlite3_stmt*s=nullptr;
 if(sqlite3_prepare_v2(db_,"SELECT id,username,password_hash,role,score FROM accounts WHERE username=?1",-1,&s,nullptr)!=SQLITE_OK)return false;
 sqlite3_bind_text(s,1,u.c_str(),-1,SQLITE_TRANSIENT);bool ok=false;
 if(sqlite3_step(s)==SQLITE_ROW){out.id=sqlite3_column_int(s,0);out.username=(const char*)sqlite3_column_text(s,1);out.passwordHash=(const char*)sqlite3_column_text(s,2);out.role=(Role)sqlite3_column_int(s,3);out.score=sqlite3_column_int(s,4);ok=true;}
 sqlite3_finalize(s);return ok;
#else
 (void)u;(void)out;return false;
#endif
}
bool SqliteStore::UpsertAccount(const Account&a){
#if defined(GTANET_WITH_SQLITE)
 if(!db_)return false;sqlite3_stmt*s=nullptr;const char*q="INSERT INTO accounts(id,username,password_hash,role,score) VALUES(?1,?2,?3,?4,?5) ON CONFLICT(username) DO UPDATE SET password_hash=excluded.password_hash,role=excluded.role,score=excluded.score";
 if(sqlite3_prepare_v2(db_,q,-1,&s,nullptr)!=SQLITE_OK)return false;sqlite3_bind_int(s,1,a.id);sqlite3_bind_text(s,2,a.username.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,3,a.passwordHash.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_int(s,4,(int)a.role);sqlite3_bind_int(s,5,a.score);bool ok=sqlite3_step(s)==SQLITE_DONE;sqlite3_finalize(s);return ok;
#else
 (void)a;return false;
#endif
}
bool SqliteStore::AddBan(const std::string&k,const std::string&r,uint64_t e){
#if defined(GTANET_WITH_SQLITE)
 if(!db_)return false;sqlite3_stmt*s=nullptr;if(sqlite3_prepare_v2(db_,"INSERT OR REPLACE INTO bans(key,reason,expires_ms) VALUES(?1,?2,?3)",-1,&s,nullptr)!=SQLITE_OK)return false;
 sqlite3_bind_text(s,1,k.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_text(s,2,r.c_str(),-1,SQLITE_TRANSIENT);sqlite3_bind_int64(s,3,(sqlite3_int64)e);bool ok=sqlite3_step(s)==SQLITE_DONE;sqlite3_finalize(s);return ok;
#else
 (void)k;(void)r;(void)e;return false;
#endif
}
bool SqliteStore::IsBanned(const std::string&k,uint64_t now){
#if defined(GTANET_WITH_SQLITE)
 if(!db_)return false;sqlite3_stmt*s=nullptr;if(sqlite3_prepare_v2(db_,"SELECT expires_ms FROM bans WHERE key=?1",-1,&s,nullptr)!=SQLITE_OK)return false;sqlite3_bind_text(s,1,k.c_str(),-1,SQLITE_TRANSIENT);bool b=false;
 if(sqlite3_step(s)==SQLITE_ROW){auto e=(uint64_t)sqlite3_column_int64(s,0);b=e==0||e>now;}sqlite3_finalize(s);return b;
#else
 (void)k;(void)now;return false;
#endif
}
}