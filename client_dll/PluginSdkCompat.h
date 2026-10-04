#pragma once
#ifdef GTANET_WITH_PLUGIN_SDK
#include <plugin.h>
#include <game_sa/CPed.h>
#include <game_sa/CWorld.h>
#include <game_sa/CStreaming.h>
#include <game_sa/CVector.h>
#include <game_sa/CMatrix.h>

namespace gtanet::sdk {
// Central compatibility layer for the current DK22Pac/plugin-sdk SA API.
CPed* SpawnPed(unsigned short skin, float x, float y, float z, float headingDeg, float health);
void ApplyPed(CPed* ped, float x, float y, float z, float headingDeg, float health);
void DestroyPed(CPed* ped);
}
#endif
