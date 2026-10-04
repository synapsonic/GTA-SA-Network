#include "PluginSdkRevision.h"
#if defined(GTANET_WITH_PLUGIN_SDK)
#include <cstdint>
static_assert(sizeof(void*)==4,"GTA SA 1.0 US client DLL must be built Win32/x86, not x64.");
#include <plugin.h>
#include <game_sa/CPlayerPed.h>
#include <game_sa/CVehicle.h>
#endif
namespace gtanet { void PluginSdkCompileProbe(){} }
