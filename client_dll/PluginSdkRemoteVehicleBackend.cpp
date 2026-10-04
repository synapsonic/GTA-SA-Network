#include "RemoteVehicleBackend.h"
#include <cmath>
#include <cstdint>

#if defined(GTANET_WITH_PLUGIN_SDK)
#include <plugin.h>
#include <game_sa/CStreaming.h>
#include <game_sa/CWorld.h>
#include <game_sa/CVehicle.h>
#include <game_sa/CAutomobile.h>
#include <game_sa/CBike.h>
#include <game_sa/CBoat.h>
#include <game_sa/CModelInfo.h>
#endif

namespace gtanet {

#if defined(GTANET_WITH_PLUGIN_SDK)

static bool ValidVehicleModel(uint16_t model) {
    return model >= 400 && model <= 611 && CModelInfo::GetModelInfo(model) != nullptr;
}

static bool EnsureModel(uint16_t model) {
    if (!ValidVehicleModel(model)) return false;
    if (!CStreaming::ms_aInfoForModel[model].m_nLoadState) {
        CStreaming::RequestModel(model, GAME_REQUIRED);
        CStreaming::LoadAllRequestedModels(false);
    }
    return CStreaming::ms_aInfoForModel[model].m_nLoadState != 0;
}

/*
 * plugin-sdk revisions differ in the safest public construction path for
 * concrete GTA vehicle subclasses. Keep that revision-specific constructor
 * in ONE function. The rest of the multiplayer code never depends on it.
 *
 * Define GTANET_PLUGINSDK_VEHICLE_FACTORY and provide CreateSdkVehicle()
 * in PluginSdkVehicleFactory.cpp for the exact plugin-sdk revision you use.
 */
#if defined(GTANET_PLUGINSDK_VEHICLE_FACTORY)
CVehicle* CreateSdkVehicle(uint16_t model);
#else
static CVehicle* CreateSdkVehicle(uint16_t) { return nullptr; }
#endif

class PluginSdkVehicleBackend final : public RemoteVehicleBackend {
public:
    void* Spawn(const VehicleSpawnPacket& s) override {
        if (!EnsureModel(s.modelId)) return nullptr;
        CVehicle* v = CreateSdkVehicle(s.modelId);
        if (!v) return nullptr;
        v->SetPosn(s.x, s.y, s.z);
        v->SetHeading(s.heading);
        v->m_fHealth = s.health;
        v->m_nPrimaryColor = s.color1;
        v->m_nSecondaryColor = s.color2;
        CWorld::Add(v);
        return v;
    }

    void Apply(void* handle, const VehicleStatePacket& s) override {
        auto* v = static_cast<CVehicle*>(handle);
        if (!v) return;
        v->SetPosn(s.x, s.y, s.z);
        v->SetHeading(s.heading);
        v->m_vecMoveSpeed.x = s.vx;
        v->m_vecMoveSpeed.y = s.vy;
        v->m_vecMoveSpeed.z = s.vz;
        v->m_fHealth = s.health;
    }

    void Destroy(void* handle) override {
        auto* v = static_cast<CVehicle*>(handle);
        if (!v) return;
        CWorld::Remove(v);
        delete v;
    }
};

#else

class PluginSdkVehicleBackend final : public RemoteVehicleBackend {
public:
    void* Spawn(const VehicleSpawnPacket&) override { return nullptr; }
    void Apply(void*, const VehicleStatePacket&) override {}
    void Destroy(void*) override {}
};

#endif

RemoteVehicleBackend* CreateRemoteVehicleBackend() {
    return new PluginSdkVehicleBackend();
}
}