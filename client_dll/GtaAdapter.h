#pragma once
#include "../shared/Protocol.h"
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <string>

class GtaAdapter {
public:
    bool Initialize();
    bool IsReady() const { return ready_; }
    bool ReadLocalState(gtanet::PlayerStatePacket& out);
    void OnRemoteJoin(uint32_t playerId, const std::string& name, uint16_t skin);
    void OnRemoteLeave(uint32_t playerId);
    void PushRemoteState(const gtanet::PlayerStatePacket& state);
    void TickRemotePlayers(float dtSeconds);
private:
    struct Remote {
        gtanet::PlayerStatePacket previous{}, target{};
        std::string name; uint16_t skin=0;
        float alpha=1.0f; bool hasState=false; uintptr_t ped=0; bool streamed=false;
    };
    bool DetectSupportedBuild();
    bool ReadLocalPed(gtanet::PlayerStatePacket& out);
    bool CanUseGameEntityApi() const;
    void EnsureRemotePed(uint32_t id, Remote& remote, const gtanet::PlayerStatePacket& render);
    void ApplyRemotePed(uint32_t id, Remote& remote, const gtanet::PlayerStatePacket& render);
    void DestroyRemotePed(Remote& remote);
    void DrawRemoteNameTag(uint32_t id, const Remote& remote, const gtanet::PlayerStatePacket& render);
    bool ready_=false, entityApiEnabled_=false;
    std::mutex mutex_;
    std::unordered_map<uint32_t,Remote> remotes_;
};
