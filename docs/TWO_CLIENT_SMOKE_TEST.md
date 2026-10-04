# V2.3 two-client smoke test

Prerequisites:
- two separate GTA San Andreas 1.0 US installations/process environments;
- the same V2.3 client DLL in both;
- one V2.1+ dedicated server;
- a plugin-sdk checkout matching the client build.

Expected sequence:
1. Start the dedicated server and confirm it listens on the configured port.
2. Launch client A. Confirm `gtanet-client.log` records initialization and connection.
3. Launch client B with a different nickname.
4. Confirm both clients receive distinct player IDs.
5. Walk client A. Client B should receive snapshots and update the remote ped.
6. Repeat B -> A.
7. Enter/exit a vehicle and verify ownership/seat events.
8. Disconnect B and confirm its remote ped/vehicle occupancy is cleaned on A.

Failure isolation:
- no Welcome: transport/handshake;
- Welcome but no remote identity: protocol dispatch;
- identity but no ped: plugin-sdk ped factory/streaming;
- ped exists but does not move: snapshot/interpolation bridge;
- vehicle exists but seat fails: revision-specific seat task adapter.

Attach both `gtanet-client.log` files and the server log when diagnosing a failed run.
