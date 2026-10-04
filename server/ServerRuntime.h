#pragma once
#include "ServerConfig.h"
#include "Logger.h"
#include "ResourceRuntime.h"
#include "SqliteStore.h"
#include <atomic>
#include <memory>
namespace gtanet {
class ServerRuntime {
 ServerConfig config_;
 Logger log_;
 SqliteStore db_;
 std::unique_ptr<ResourceRuntime> resources_;
 std::atomic<bool> running_{false};
public:
 explicit ServerRuntime(ServerConfig c):config_(std::move(c)),log_("server.log"){}
 bool Start();
 void Tick(uint64_t nowMs);
 void Stop();
 bool Running()const{return running_.load();}
 const ServerConfig& Config()const{return config_;}
};
}