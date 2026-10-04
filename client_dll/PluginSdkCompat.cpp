#include "PluginSdkCompat.h"
#ifdef GTANET_WITH_PLUGIN_SDK
#include <algorithm>
namespace gtanet::sdk {
CPed* SpawnPed(unsigned short skin, float x, float y, float z, float headingDeg, float health) {
    // Standard SA ped models are 0..311. Fall back to a known pedestrian model.
    int model = skin <= 311 ? static_cast<int>(skin) : 7;
    CStreaming::RequestModel(model, GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    if (!CStreaming::HasModelLoaded(model)) return nullptr;

    auto* ped = new CPed(PED_TYPE_CIVMALE);
    if (!ped) return nullptr;
    ped->SetModelIndex(model);
    ped->SetPosn(x, y, z);
    ped->SetHeading(headingDeg * 0.01745329251994329577f);
    ped->m_fHealth = std::clamp(health, 0.0f, 1000.0f);
    CWorld::Add(ped);
    CStreaming::SetModelIsDeletable(model);
    return ped;
}
void ApplyPed(CPed* ped, float x, float y, float z, float headingDeg, float health) {
    if (!ped) return;
    ped->SetPosn(x, y, z);
    ped->SetHeading(headingDeg * 0.01745329251994329577f);
    ped->m_fHealth = std::clamp(health, 0.0f, 1000.0f);
}
void DestroyPed(CPed* ped) {
    if (!ped) return;
    CWorld::Remove(ped);
    delete ped;
}
}
#endif
