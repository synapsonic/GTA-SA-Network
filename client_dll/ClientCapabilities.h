#pragma once
#include <string>
namespace gtanet {
struct ClientCapabilities {
 bool pluginSdk=false, gameThread=false, localPlayer=false, localVehicle=false;
 bool remotePeds=false, remoteVehicles=false, remoteSeats=false;
 std::string Summary() const;
};
ClientCapabilities DetectClientCapabilities();
}