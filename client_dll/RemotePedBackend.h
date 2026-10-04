#pragma once
#include <cstdint>
#include <string>
#include "../shared/Protocol.h"

// Game-facing contract. All methods must be called from GTA's game thread.
// The plugin-sdk implementation belongs behind this interface; the network
// layer never owns GTA entities directly.
class RemotePedBackend {
public:
    virtual ~RemotePedBackend() = default;
    virtual uintptr_t Spawn(uint32_t networkId, uint16_t skin, const gtanet::PlayerStatePacket& initial) = 0;
    virtual void Apply(uintptr_t ped, const gtanet::PlayerStatePacket& state) = 0;
    virtual void Despawn(uintptr_t ped) = 0;
    virtual void DrawNameTag(uintptr_t ped, const std::string& name, float health) = 0;
};
