#pragma once
// Single place for revision-dependent GTA/plugin-sdk capabilities.
// V2.3 intentionally refuses to guess constructor/task signatures.
#if defined(GTANET_WITH_PLUGIN_SDK)
 #define GTANET_GTA_SA_10_US 1
#endif
namespace gtanet {
struct PluginSdkRevision {
 static constexpr const char* Game="GTA San Andreas";
 static constexpr const char* Target="1.0 US / Win32";
 static constexpr bool Requires32Bit=true;
};
}