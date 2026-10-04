/*
Copy this file to PluginSdkVehicleFactory.cpp and adapt the constructors to
the exact plugin-sdk revision installed on your machine.

This is intentionally NOT compiled by default: constructor signatures for
CAutomobile/CBike/CBoat can differ between SDK revisions and guessing them
would turn a network bug into a GTA crash.

#include <plugin.h>
#include <game_sa/CVehicle.h>
#include <game_sa/CAutomobile.h>
#include <game_sa/CBike.h>
#include <game_sa/CBoat.h>

namespace gtanet {
CVehicle* CreateSdkVehicle(uint16_t model) {
    // Determine the model's vehicle type using your plugin-sdk revision.
    // Construct the matching concrete class, set its model index, then return it.
    return nullptr;
}
}
*/