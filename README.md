# GTA SA Network Launcher — v0.4

Independent multiplayer prototype for a legally owned **GTA San Andreas 1.0 US 32-bit** installation. No Rockstar or SA-MP files/code are included.

## What v0.4 adds

- GTA 1.0 US adapter profile isolated in `client_dll/GtaAdapter.cpp`.
- Runtime host/build sanity checks before enabling memory access.
- Read-only extraction of the local player's world position, heading and health.
- 20 Hz client snapshots sent through the existing authoritative relay server.
- Remote-player state buffers with shortest-path heading interpolation.
- Explicit `EnsureRemotePed / ApplyRemotePed / DestroyRemotePed` integration boundary.
- Remote GTA entity mutation intentionally remains disabled until it can be marshalled onto GTA's main/game thread.

## Architecture

`gta_sa.exe -> gtasa_network.dll -> GtaAdapter -> NetworkClient -> gtasa_server`

Incoming remote snapshots follow the inverse path and are interpolated before presentation.

## Build

Use Visual Studio 2022 with the x86 C++ toolchain and CMake:

```powershell
cmake -S . -B build -A Win32
cmake --build build --config Release
```

The GTA client DLL must be 32-bit. The dedicated server/launcher can also be built from the same solution.

## Configuration

Copy `config/network.ini.example` beside the launcher as `network.ini` and set the server host, port and player name.

## Important compatibility note

This release targets the classic **1.0 US 32-bit executable layout**. Do not use the memory adapter with Steam/Definitive Edition/modified executables. The adapter performs sanity checks and disables itself if the expected memory location is unavailable, but exact executable verification should be strengthened before public distribution.

## Current multiplayer scope

The server can accept multiple clients and relay player snapshots. A running 1.0 US client can now supply its actual local position/heading/health to that network layer. Remote snapshots are received and interpolated, but **remote peds are not spawned yet**.

## Next milestone — v0.5

The next safe architectural step is a GTA-main-thread command bridge. It will:

1. queue remote join/state/leave commands from the networking thread;
2. execute GTA engine operations only from the game thread;
3. create one non-player ped per remote player;
4. update position/heading/health using interpolated snapshots;
5. destroy remote entities on disconnect;
6. add model loading and distance-based streaming.

After that: on-foot controls/animations, vehicles, weapons/damage, chat, spawning and server scripting.

## V0.7 - remote-player streaming layer

V0.7 adds the remote-player lifecycle required before visible multiplayer entities can safely be enabled:

- interpolation and distance-based streaming (180 m spawn / 220 m despawn hysteresis);
- a dedicated game-facing entity API boundary (`EnsureRemotePed`, `ApplyRemotePed`, `DestroyRemotePed`);
- network threads no longer perform remote-player ticks directly;
- `GameThreadBridge` scaffold for a verified per-frame callback;
- all CPed creation/writes remain safety-gated until the exact GTA SA 1.0 US executable and internal symbols are verified.

### Why the visible CPed calls are gated

Calling guessed GTA internal function addresses can corrupt memory or crash the game. The networking and streaming implementation is complete enough to accept remote snapshots, but the final game-engine bridge must be bound against the exact executable build (or a compatible plugin-sdk integration) before `entityApiEnabled_` is switched on.

### Next integration target

Bind a verified GTA SA 1.0 US game-loop callback and CPed create/destroy/update API, then enable `entityApiEnabled_`. Once that adapter is supplied, the existing streaming layer will create remote players inside 180 m, interpolate their movement, and remove them outside 220 m.

# V0.7 — plugin-sdk bridge

V0.7 introduces an optional **plugin-sdk** integration layer for GTA San Andreas 1.0 US (32-bit). It does not include Rockstar files or plugin-sdk itself.

## What changed

- `PluginSdkBridge` provides the GTA-specific integration boundary.
- With `GTANET_WITH_PLUGIN_SDK=ON`, remote-player interpolation is ticked from `plugin::Events::gameProcessEvent`, i.e. the GTA game thread rather than the networking thread.
- The previous Win32 timer remains as a fallback build mode, but GTA entity mutation stays safety-gated in that mode.
- Network protocol/server remain independent of GTA internals.

## Building with plugin-sdk

Clone/install plugin-sdk separately, then generate a **Win32 / x86** build:

```powershell
cmake -S . -B build -A Win32 `
  -DGTANET_WITH_PLUGIN_SDK=ON `
  -DPLUGIN_SDK_DIR=C:/dev/plugin-sdk
cmake --build build --config Release
```

The exact plugin-sdk checkout/toolchain can require additional include/library settings depending on how plugin-sdk was installed. Do not use a 64-bit target for classic GTA SA 1.0 US.

## Current multiplayer pipeline

`GTA local player -> PlayerStatePacket -> dedicated server -> remote snapshots -> interpolation -> gameProcessEvent -> GTA adapter`

The remaining V0.7 entity layer is responsible for owning actual remote `CPed` instances: model request/load, creation, world insertion, matrix/heading/health updates, animation mapping, and safe destruction. Keeping this layer separate prevents networking code from directly manipulating GTA memory.

## Compatibility

Target: GTA San Andreas 1.0 US, 32-bit. Other executables must not be assumed compatible. Always test on a clean modded copy of the game and keep saves backed up.


## V0.7 - Remote player identity and entity pipeline

Protocol version 3 adds `skin` to Hello/PlayerJoin. The server now propagates a player's display name and skin once at join time, while high-frequency PlayerState packets remain compact.

The client maintains one remote record per network player containing identity, interpolation state and a future GTA entity handle. Streaming remains 180 m in / 220 m out. `RemotePedBackend.h` is the game-facing boundary for Spawn / Apply / Despawn / DrawNameTag and is only intended to be invoked from GTA's game thread.

### Why the concrete CPed constructor is not hard-coded

plugin-sdk revisions and GTA SA executable builds differ in their exposed constructors/helpers. V0.7 therefore does not ship guessed internal addresses or an unsafe fake CPed constructor. Bind `PluginSdkRemotePedBackend.cpp` to the exact plugin-sdk revision you compile against, then enable `GTANET_WITH_PLUGIN_SDK=ON`. This keeps the networking/server code stable and prevents a wrong engine call from corrupting the game process.

### Next integration checkpoint

Once the selected plugin-sdk checkout is present, implement the five calls documented in `PluginSdkRemotePedBackend.cpp`: model streaming, CPed creation + CWorld insertion, transform/health application, nametag projection, and CWorld removal/destruction. After that, two connected clients can materialize each other's remote actors; vehicle sync and combat can then be layered on the same protocol.

## V2.3 - concrete remote CPed backend

V2.3 wires the remote-player pipeline to the current DK22Pac/plugin-sdk SA API when `GTANET_WITH_PLUGIN_SDK=ON`.

The compatibility code is isolated in `client_dll/PluginSdkCompat.{h,cpp}`. It requests/loads the remote skin model, creates a non-player `CPed`, adds it to `CWorld`, updates position/heading/health during `gameProcessEvent`, and removes/deletes it on despawn/disconnect.

Build example (Visual Studio 2022, Win32):

```powershell
cmake -S . -B build -A Win32 -DGTANET_WITH_PLUGIN_SDK=ON -DPLUGIN_SDK_DIR=C:/dev/plugin-sdk
cmake --build build --config Release
```

Target: GTA San Andreas 1.0 US 32-bit. Keep a clean game backup. This project does not contain Rockstar assets or SA-MP code.

### Current multiplayer milestone

With a compatible plugin-sdk checkout and GTA build, the client now has the code path required to materialize remote network players as GTA peds and update their interpolated transform/health. Nametag drawing remains a separate render hook for the next milestone. Vehicle sync, weapons, animation replication and authoritative damage are not implemented yet.


## V2.3 - on-foot gameplay state

Protocol version 4 adds compact replicated fields for animation, animation flags, equipped weapon, weapon state and movement state. Identity (nickname/skin) remains in the join handshake and is not retransmitted at 20 Hz.

The remote-player pipeline now has explicit boundaries for nametag/health rendering and animation/weapon application. Concrete rendering and animation calls remain isolated in `PluginSdkCompat` so plugin-sdk revision differences do not leak into networking.

### Compatibility
V2.3 clients and servers must be used together. Older protocol versions are rejected by `ValidHeader()` instead of decoding packets with a mismatched layout.

### Next implementation targets
- map GTA local weapon slot/current weapon into `weapon`;
- map locomotion/animation IDs into `animation` and `movementState`;
- apply remote animation associations through plugin-sdk;
- render nickname and health bar using GTA's render event;
- add authoritative shot/damage events separately from visual weapon state.


## V2.3 — Vehicle networking

Protocol version **5** adds a dedicated vehicle channel:

- `VehicleSpawn`: network ID, owner, GTA model, colors, transform and health.
- `VehicleState`: transform, linear velocity, health, current driver and sequence.
- `VehicleDestroy`: removes a streamed network vehicle.
- `VehicleEnter` / `VehicleExit`: seat ownership events.
- `RemoteVehicleManager`: client-side registry for streamed vehicles.
- `RemoteVehicleBackend`: the only GTA-specific boundary used to create/apply/delete `CVehicle` instances.

### Authority model

The server must accept `VehicleState` only from the vehicle's current `ownerPlayerId`.
On an ownership transfer (driver changes, owner disconnects, etc.) the server chooses the
new owner and broadcasts the resulting state. Clients must never be allowed to claim an
arbitrary vehicle simply by sending a state packet.

### plugin-sdk vehicle bridge

`PluginSdkRemoteVehicleBackend.cpp` is deliberately the compatibility boundary. The network
protocol and vehicle registry are complete, but actual `CAutomobile`/`CBike`/`CBoat` creation
must be bound to the exact plugin-sdk revision used to build the DLL. Do not replace this
with guessed GTA addresses.

Recommended implementation order:

1. Load the GTA model with `CStreaming`.
2. Select the correct concrete vehicle class for the model.
3. Add it to `CWorld` on the GTA game thread.
4. Apply transform, velocity, health and colors from `VehicleState`.
5. Attach remote players to seats from `VehicleEnter`.
6. Remove the entity from `CWorld` before deleting it.

The wire protocol is intentionally independent from GTA internals, so this bridge can be
changed without breaking the server or launcher.


## V2.3 — GTA vehicle bridge

V2.3 turns the vehicle layer into a plugin-sdk-backed runtime boundary.

Implemented:
- GTA vehicle model validation (`400..611`) before spawning.
- Model request/loading through `CStreaming`.
- Remote vehicle transform, heading, move speed, health and primary/secondary colors.
- `CWorld::Add` / `CWorld::Remove` lifecycle on the game-thread-side backend.
- `VehicleOccupancy` registry for driver/passenger seat assignments.
- Safe standalone fallback when plugin-sdk is disabled.
- A single revision-specific factory (`PluginSdkVehicleFactory.cpp`) for GTA constructors.

### One required compatibility step

The concrete `CAutomobile` / `CBike` / `CBoat` constructors are intentionally not guessed.
Copy `client_dll/PluginSdkVehicleFactory.example.cpp` to
`PluginSdkVehicleFactory.cpp`, implement `CreateSdkVehicle()` against the exact plugin-sdk
revision installed locally, add it to the DLL target and define
`GTANET_PLUGINSDK_VEHICLE_FACTORY=1`.

Keeping this one function revision-specific prevents GTA memory/layout assumptions from
leaking through the multiplayer code.

### Next milestone

V2.3 can wire `VehicleEnter`/`VehicleExit` into GTA tasks so remote peds occupy actual seats,
then add local vehicle capture and server ownership hand-off when the local player becomes
the driver.


## V2.3 — Seats and authoritative vehicle ownership

Protocol version **6** introduces explicit ownership negotiation.

### Flow

1. The local GTA bridge detects that the player entered a network vehicle.
2. Client sends `VehicleEnter` and, for seat `0` (driver), `VehicleClaim`.
3. Server validates that no other live driver owns that vehicle.
4. Server broadcasts `VehicleOwnership(vehicleId, ownerPlayerId, revision)`.
5. Only that owner may publish `VehicleState`.
6. Other clients interpolate the received vehicle state and attach the remote player to the corresponding seat.
7. On driver exit/disconnect, server clears ownership and increments the ownership revision.

`revision` prevents an old ownership packet arriving late from taking control back.

### New components

- `server/VehicleAuthority.h` — authoritative ownership/driver registry.
- `client_dll/VehicleOwnership.h` — client-side ownership cache with revision protection.
- `client_dll/LocalVehicleBridge.*` — isolated GTA/plugin-sdk boundary for detecting the local player's vehicle and capturing its state.
- Existing `VehicleOccupancy` remains the seat registry.

### Important integration boundary

The exact mapping between a GTA `CVehicle*` and the multiplayer `vehicleId`, plus the local-player accessor,
depends on the selected plugin-sdk revision and on how vehicles are registered when they spawn. Those operations
remain isolated in `LocalVehicleBridge.cpp` rather than using guessed memory addresses.

### Server rule

Never accept a `VehicleState` because its packet says `ownerPlayerId == sender`.
Look up the socket's authenticated `playerId`, then call `VehicleAuthority::CanPublish(vehicleId, playerId)`.
The server is the authority.

### Next milestone

V2.3 can finish the GTA-facing seat adapter: assign remote peds to actual driver/passenger seats, detect local
enter/exit transitions each frame, publish owned vehicle snapshots at a fixed rate, and smooth remote vehicles
with interpolation/extrapolation.


## V2.3 — Smooth vehicle synchronization

Protocol version **7** adds `serverTimeMs` to vehicle snapshots.

New runtime pieces:

- `VehicleSyncController`: detects local enter/exit transitions and emits `VehicleEnter`, `VehicleExit` and `VehicleClaim`.
- Owned driver vehicles are sampled at **20 Hz** (50 ms).
- `VehicleSnapshotBuffer`: keeps up to 32 ordered snapshots per remote vehicle.
- Rendering samples approximately **100 ms behind** the newest server timeline.
- Missing short-term data is extrapolated using velocity for at most **150 ms**; after that the last authoritative state is held.
- `RemoteSeatAdapter`: game-thread boundary for putting remote peds in driver/passenger seats.

### Why buffer instead of applying packets directly?

Packets do not arrive at perfectly regular intervals. Applying each packet immediately creates visible micro-teleports.
The buffer reconstructs a smooth state between two authoritative snapshots. Short extrapolation hides a single late
packet without allowing a client vehicle to drift indefinitely.

### Remaining plugin-sdk binding

`LocalVehicleBridge` still needs the exact project's `CVehicle* <-> vehicleId` registry and local-player accessor.
`RemoteSeatAdapter` needs the matching remote `CPed*` and `CVehicle*` registries plus the seat/task API from the
selected plugin-sdk revision. These are intentionally isolated instead of relying on guessed memory addresses.

### Next milestone

V2.3 can add combat networking: aim state, shot events, server-side validation, damage/death/respawn and weapon
synchronization, while keeping damage authority separate from purely visual animation packets.


## V2.3 — Multiplayer combat foundation

Protocol version **8** adds:
`AimState`, `Shot`, `DamageRequest`, `PlayerHealth`, `PlayerDeath`, and `PlayerRespawn`.

The design deliberately separates visual prediction from authoritative gameplay:

- Aim and muzzle/shot events can be shown immediately to other clients.
- A client never directly sets another player's health.
- `CombatAuthority` checks the authenticated socket player, alive state, weapon fire cadence,
  approximate attacker/victim range and clamps requested damage.
- Accepted damage is converted into authoritative `PlayerHealth`; lethal damage becomes `PlayerDeath`.
- Respawn restores authoritative state and is broadcast with `PlayerRespawn`.

This is a foundation, not a complete anti-cheat. A production server should retain position history and perform
lag-compensated hit/raycast validation rather than trusting a client-provided hit point.

### New client boundary

`CombatBridge` contains all plugin-sdk-specific access to the local weapon/camera and remote ped visual tasks.
`CombatSyncController` publishes aim at 20 Hz and discrete shot events.

### Next milestone

V2.3 can implement lag compensation: server position history, rewind to the shooter's timestamp, ray-versus-player
hitboxes, weapon-specific damage tables, respawn timers and a synchronized kill feed/chat.


## V2.3 — Lag compensation and kill feed

Protocol version **9** adds `KillFeed`.

Server components added:

- `LagCompensation`: keeps approximately 1.2 seconds of timestamped player positions.
- Rewind requests are capped to **350 ms** so a client cannot ask the server to validate a shot arbitrarily far in the past.
- Linear interpolation reconstructs a victim position between two authoritative samples.
- `RayHitsPlayer` performs a lightweight ray-versus-player-volume check.
- `WeaponRules`: server-owned weapon damage, range and minimum fire interval.
- `RespawnManager`: schedules dead players for respawn (default 5 seconds).
- `KillFeed`: client-side rolling feed with six entries and eight-second lifetime.

### Recommended authoritative shot path

1. Receive `Shot` from the authenticated player's socket.
2. Validate cadence using the server weapon table.
3. Convert/clamp the client shot timestamp to the server timeline.
4. Rewind candidate victims with `LagCompensation::Rewind`.
5. Test the shot ray against rewound player volumes.
6. Apply the server's weapon damage — not client-provided damage.
7. Broadcast `PlayerHealth`; on lethal damage broadcast `PlayerDeath` and `KillFeed`.
8. Schedule respawn with `RespawnManager`.
9. Broadcast `PlayerRespawn` when due.

The simple player volume is intentionally inexpensive. A later version can use multiple body volumes/headshots,
vehicle occupants, world obstruction tests and more precise GTA collision geometry.

### Next milestone

V2.3 can add synchronized chat/commands, player list/scoreboard, server name/max players, nick validation,
join/leave messages, ping display and a small server configuration file — the pieces that make the project feel
like a complete multiplayer platform rather than only a synchronization prototype.


## V2.3 — Chat, commands and scoreboard

Protocol version **10** adds the social/server layer:

- `ChatMessage` — global player chat, maximum 160 bytes.
- `SystemMessage` — server-generated notices such as joins/leaves.
- `Command` — slash commands routed to the dedicated server.
- `ServerInfo` — server name, slots, online count and password flag.
- `ScoreboardRequest` / `ScoreboardEntry` — ID, nickname, ping and score independent of world streaming.
- `server.cfg.example` — name, port, max players, optional password, welcome message and join/leave announcements.

Server helpers:
- `ServerConfig` parses the simple `key=value` configuration.
- `ChatService` rejects oversized/control-character messages and applies a 600 ms per-player chat rate limit.
- `CommandRouter` parses `/command arg1 arg2`.
- Admin privileges are intentionally not based on nickname; authentication should be added before privileged commands.

Client UI state:
- `SocialUi` retains the latest chat lines and scoreboard rows.
- `SocialUiBridge.cpp` documents the GTA-thread rendering/input boundary: **T** for chat and held **TAB** for scoreboard.

### Suggested built-in commands

`/help`, `/players`, `/ping`, `/kill`, `/respawn`.

### Next milestone

V2.3 can add accounts/authentication, persistent player data, SQLite/MySQL storage, admin/RCON permissions,
bans/kicks, server-side scripting hooks and a gamemode/resource system.


## V2.3 — Accounts, administration and gamemode/resources

Protocol version **11** adds authentication results and administrative actions.

### Persistence and authentication

`AccountStore` provides a deliberately small file-backed persistence boundary for account id, username,
password-KDF hash, role and score. **Plaintext passwords must never be stored or transmitted.**
`AuthService` binds a successfully authenticated account to the network `playerId`.

Roles: Player, Moderator, Admin and Owner.

The included store is useful for development. A production deployment can replace the same boundary with
SQLite/MySQL/PostgreSQL without changing gameplay code.

### Administration

`AdminAction` supports kick, ban and unban requests. Permission checks belong on the server via
`AuthService::AtLeast`; never trust a role or nickname supplied by a client. `BanStore` supports permanent
or expiry-based bans. Network/IP/account keys can be chosen by the host according to its policy.

### Gamemodes and resources

`GameMode` exposes stable hooks:
`OnStart`, `OnStop`, `OnTick`, `OnPlayerConnect`, `OnPlayerDisconnect`, `OnPlayerDeath`, `OnCommand`.

`ResourceManager` owns independent resources with Start/Stop/Tick lifecycle. `ExampleFreeroamGameMode.cpp`
shows the intended separation between multiplayer engine and game rules.

This version intentionally uses a C++ API rather than embedding an unsafe scripting VM directly into the
network core.

### Next milestone

V2.3 can embed Lua as a sandboxed scripting layer over a narrow server API: players, vehicles, chat, timers,
commands and spawn points; add resource manifests and hot reload so server owners can build gamemodes without
recompiling the dedicated server.


## V2.3 — Sandboxed Lua resources

The dedicated server now has an optional Lua 5.4 resource layer.

Enable it with CMake option `GTANET_WITH_LUA=ON`. Without Lua installed the networking/server project can still
be built with scripting disabled.

Each resource owns an independent Lua state and a simple `resource.cfg` manifest. The example
`resources/freeroam` implements `/heal` and `/spawncar`.

### Sandbox

Only Lua base, math, string and table libraries are opened by default. `io`, `os`, `package` and `debug` are not
opened, so a downloaded gamemode does not automatically receive filesystem, shell or native-module access.

Exposed API:
- `server.broadcast(text)`
- `server.message_player(playerId, text)`
- `player.set_pos(playerId, x, y, z, heading)`
- `player.set_health(playerId, health)`
- `vehicle.create(model, x, y, z, heading)`
- `vehicle.destroy(vehicleId)`

Hooks currently supported by the wrapper include `on_start`, `on_stop`, `on_tick` and `on_command`; the generic
event boundary can also be wired to connect/disconnect/death events from the server core.

### Next milestone

V2.3 can add the resource runtime around this layer: directory discovery, dependency ordering, start/stop/reload
commands, file-change hot reload, timers and event subscriptions, plus Lua APIs for player names, weapons,
vehicles, spawn points and score.


## V2.3 — Resource runtime and hot reload

`ResourceRuntime` now turns the Lua layer into a manageable server subsystem.

Features:
- scans the `resources/` directory for `resource.cfg`;
- tracks `Stopped`, `Starting`, `Running` and `Failed` state;
- supports `Start`, `Stop` and `Reload`;
- supports comma-separated `depends=` dependencies;
- refuses to start a resource while a dependency is unavailable;
- polls resource file modification times and performs controlled stop/start hot reload;
- keeps the resource's Lua state isolated across reloads;
- includes a generic `ScriptTimers` scheduler;
- `ResourceCommands.h` gates start/stop/reload behind the Admin role.

Example `welcome` depends on `freeroam`, demonstrating dependency ordering.

### Suggested server loop

At startup:
1. create `ScriptApi`;
2. create `ResourceRuntime("resources", api)`;
3. call `Discover()`;
4. start configured resources in dependency order.

Each server frame calls `ResourceRuntime::Tick(nowMs)`. File-change polling should run at a much lower frequency,
for example once per second, then call `PollHotReload()`.

### Security

Hot reload does not execute modified code in the middle of an existing Lua callback. The current resource is
stopped and its Lua state destroyed before a fresh state is created. Lua remains sandboxed as in V1.8.

### Next milestone

V2.3 can focus on making the repository actually deployable end-to-end: integrate these managers into
`server/main.cpp`, add structured logging, graceful shutdown, a real SQLite account/ban database, Docker/Linux
build instructions, Windows client build instructions, and automated protocol/unit tests.


# V2.3 — Deployable server foundation

V2.3 focuses on integration and repeatable builds rather than another gameplay subsystem.

### Server runtime
`ServerRuntime` now owns startup/shutdown, logging, optional SQLite persistence and the resource runtime.
`server/main.cpp` remains the network transport entry point; bind its existing player/vehicle registries into
the `ScriptApi` callbacks created by `ServerRuntime`.

### SQLite
Build with `GTANET_WITH_SQLITE=ON`. `SqliteStore` creates WAL-backed `accounts` and `bans` tables and uses
prepared statements for values. The older flat-file account store remains useful for development/migration.

### Tests
`tests/protocol_tests.cpp` checks protocol headers plus lag compensation, weapon rules and chat rate limiting.
Build with `GTANET_BUILD_TESTS=ON` and run `ctest`.

### Linux dedicated server
Use `scripts/build-server-linux.sh` after installing a C++ toolchain, CMake, Lua 5.4 development files and SQLite3
development files.

### Windows GTA client
Use a 32-bit Windows toolchain because GTA San Andreas 1.0 US is a 32-bit process. The provided
`scripts/build-client-windows.ps1` configures an x86 build. plugin-sdk still has to match the selected game
revision and the revision-specific vehicle/ped bindings described in earlier versions.

### Deployment boundary
The project contains no Rockstar game assets and does not redistribute GTA SA or SA-MP code. Users supply their
own compatible game installation and plugin-sdk environment.

### What remains before calling this production-ready?
The major remaining task is integration testing against a real GTA SA 1.0 US/plugin-sdk setup: complete the
revision-specific ped/vehicle/seat/local-player adapters, wire all packet handlers into the runtime, test multiple
real clients under latency/loss, then harden authentication and transport security.


# V2.3 — Integration pass

This release fixes issues discovered by actually configuring and compiling the Linux server path.

- fixed the malformed `VehicleExitPacket` section in `Protocol.h`; protocol version is now 12;
- moved root CMake build options before subdirectories, so server/client/launcher can truly be enabled independently;
- added `SocketCompat.h` and made the dedicated TCP server build on Windows or POSIX/Linux;
- connected `ServerRuntime`, chat validation, player position history, vehicle authority, aim/shot forwarding and scoreboard handling to `server/main.cpp`;
- server overwrites client-supplied player IDs with the authenticated connection ID and only accepts vehicle state from the current vehicle owner;
- fixed the client CMake source list so the vehicle/combat/social source files added in previous versions are part of the DLL target;
- Linux build script now defaults to dependency-free Lua/SQLite OFF, with tests ON. Enable those optional integrations after the base build passes.

The Windows GTA/plugin-sdk bridge is still not claimed as runtime-tested here: final ped/vehicle/seat bindings require the exact 32-bit plugin-sdk revision and a real GTA SA 1.0 US installation.


## V2.3 — GTA client integration pass

This pass concentrates the remaining GTA-specific work behind explicit plugin-sdk capability boundaries.

Added:
- `ClientCapabilities` startup capability matrix;
- plugin-sdk local vehicle detection using the local `CPlayerPed`;
- local vehicle position, heading, velocity and health capture;
- `VehicleIdRegistry` mapping network vehicle IDs to `CVehicle*`;
- `PluginSdkEntityRegistry` for remote ped identity;
- seat synchronization routed through a revision-specific adapter;
- explicit factory templates for the exact plugin-sdk checkout.

No executable addresses were invented to fake compatibility. Concrete vehicle/ped constructors and task-based
seat assignment still depend on the exact plugin-sdk revision selected for the Windows x86 build.

The Linux dedicated server from V2.1 remains independently buildable/testable.

### V2.3 target
Use an actual plugin-sdk checkout in a Windows x86 build environment, implement the three revision adapters,
compile the DLL, then run an in-game two-client smoke test with diagnostic logging for join, ped spawn, movement,
vehicle enter/exit and disconnect cleanup.


## V2.3 — Windows/plugin-sdk integration kit

V2.3 makes the final GTA-specific integration reproducible instead of pretending a particular plugin-sdk
revision is available in every environment.

- strict Win32/x86 compile check when plugin-sdk is enabled;
- `PLUGIN_SDK_ROOT` validation;
- compile probe for the GTA SA headers used by the bridge;
- centralized revision/capability definitions;
- persistent `gtanet-client.log`;
- PowerShell configure/build script;
- exact adapter checklist;
- two-client smoke-test procedure and failure-isolation guide.

Example Windows build:

    .\\scripts\\configure-client-windows.ps1 -PluginSdkRoot C:\\dev\\plugin-sdk

A successful Linux server build does not prove the GTA DLL works in-game. The client should only be considered
validated after the Win32/plugin-sdk build succeeds and the two-client smoke test passes.

### V2.4 target
Once the exact plugin-sdk checkout/build errors or a successful DLL build are available, V2.4 should implement
the concrete ped/vehicle/seat adapters and fix the resulting compiler/runtime issues rather than adding more
architecture.
