option(GTANET_WITH_PLUGIN_SDK "Build GTA bridge with plugin-sdk" OFF)
add_library(gtasa_network SHARED
  dllmain.cpp NetworkClient.cpp GtaAdapter.cpp GameThreadBridge.cpp PluginSdkBridge.cpp PluginSdkCompat.cpp
  PluginSdkRemotePedBackend.cpp PluginSdkRemoteVehicleBackend.cpp RemoteVehicleManager.cpp LocalVehicleBridge.cpp
  RemoteSeatAdapter.cpp CombatBridge.cpp SocialUiBridge.cpp)
target_include_directories(gtasa_network PRIVATE ../shared)
target_link_libraries(gtasa_network PRIVATE ws2_32)
set_target_properties(gtasa_network PROPERTIES OUTPUT_NAME "gtasa_network")
if(GTANET_WITH_PLUGIN_SDK)
  if(NOT DEFINED PLUGIN_SDK_DIR)
    message(FATAL_ERROR "Set PLUGIN_SDK_DIR to your plugin-sdk checkout")
  endif()
  target_compile_definitions(gtasa_network PRIVATE GTANET_WITH_PLUGIN_SDK=1)
  target_include_directories(gtasa_network PRIVATE "${PLUGIN_SDK_DIR}" "${PLUGIN_SDK_DIR}/plugin_sa")
endif()

message(STATUS "GTA client: plugin-sdk=${GTANET_WITH_PLUGIN_SDK}")
if(GTANET_WITH_PLUGIN_SDK)
  message(STATUS "GTA client target is 32-bit and requires a GTA SA 1.0 US-compatible plugin-sdk checkout")
endif()

target_sources(gtasa_network PRIVATE ClientCapabilities.cpp PluginSdkEntityRegistry.cpp PluginSdkBuildCheck.cpp)

if(GTANET_WITH_PLUGIN_SDK)
  if(NOT WIN32)
    message(FATAL_ERROR "The GTA client/plugin-sdk target is Windows-only.")
  endif()
  if(CMAKE_SIZEOF_VOID_P AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "The GTA SA client DLL must be configured as Win32/x86.")
  endif()
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
  include(FindPluginSdk)
  target_include_directories(gtasa_network PRIVATE "${PLUGIN_SDK_INCLUDE_DIR}" "${PLUGIN_SDK_ROOT}/shared")
endif()
