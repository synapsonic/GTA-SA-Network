#include "SocialUi.h"
namespace gtanet {
// Render/input integration belongs on GTA's game thread.
// Suggested controls:
//   T -> open chat input
//   Enter -> submit
//   Escape -> cancel
//   TAB (held) -> scoreboard
// Use plugin-sdk/font APIs for rendering in the selected SDK revision.
// Never execute slash commands locally unless explicitly designed as client-only;
// send them to the server as CommandPacket.
}