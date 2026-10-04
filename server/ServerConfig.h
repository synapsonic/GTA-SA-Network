#pragma once
#include <string>
#include <cstdint>
namespace gtanet {
struct ServerConfig {
    std::string name="GTA Network Server";
    uint16_t port=7777;
    uint16_t maxPlayers=32;
    std::string password;
    std::string welcome="Welcome to the server.";
    bool announceJoinLeave=true;
    static ServerConfig Load(const std::string& path);
};
}