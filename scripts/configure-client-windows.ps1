param(
  [Parameter(Mandatory=$true)][string]$PluginSdkRoot,
  [string]$Generator="Visual Studio 17 2022"
)
$ErrorActionPreference="Stop"
if (-not (Test-Path "$PluginSdkRoot\plugin_sa\plugin.h")) {
  throw "plugin.h not found. Expected: $PluginSdkRoot\plugin_sa\plugin.h"
}
cmake -S . -B build-client -G $Generator -A Win32 `
  -DGTANET_BUILD_SERVER=OFF `
  -DGTANET_BUILD_CLIENT=ON `
  -DGTANET_WITH_PLUGIN_SDK=ON `
  -DPLUGIN_SDK_ROOT="$PluginSdkRoot"
cmake --build build-client --config Release
