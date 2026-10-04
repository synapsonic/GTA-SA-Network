# Exact plugin-sdk adapters still required

V2.3 centralizes the last revision-dependent pieces. Do these against the exact checkout used by the Win32 build.

1. Remote ped factory
   - create CPed/CPlayerPed-compatible remote entity;
   - request/load model through CStreaming;
   - add/remove through CWorld;
   - bind/unbind it in PluginSdkEntityRegistry.

2. Vehicle factory
   - implement `CreateSdkVehicle(uint16_t model)`;
   - select CAutomobile/CBike/CBoat/etc. from model type;
   - use the constructor signature in your checkout;
   - do not paste raw executable addresses.

3. Seat task API
   - implement `GtaNetAssignPedSeat` and `GtaNetRemovePedFromVehicle`;
   - use the exact task API available in the checkout.

Compile-time flags:
- GTANET_PLUGINSDK_PED_FACTORY
- GTANET_PLUGINSDK_VEHICLE_FACTORY
- GTANET_PLUGINSDK_SEAT_API

Only enable a flag once its adapter compiles and passes the smoke test.
