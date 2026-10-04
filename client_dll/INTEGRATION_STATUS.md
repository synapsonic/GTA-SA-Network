# GTA client integration status — V2.2

## Implemented without revision-specific guesses
- plugin-sdk gameProcessEvent bridge
- local player bridge/fallback
- local vehicle detection via FindPlayerPed / m_pVehicle
- local vehicle transform, velocity and health capture
- network vehicle ID <-> CVehicle handle registry
- remote vehicle transform application
- capability diagnostics
- seat adapter routing through a revision-specific boundary

## Requires the exact plugin-sdk checkout
- concrete CPed factory
- concrete CVehicle subclass factory
- seat task API
- final on-screen chat/scoreboard renderer

These are deliberately compile-time capabilities rather than hard-coded GTA executable addresses.
