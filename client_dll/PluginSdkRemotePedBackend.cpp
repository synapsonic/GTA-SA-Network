#include "RemotePedBackend.h"
#ifdef GTANET_WITH_PLUGIN_SDK
// V0.7 integration boundary.
// Keep concrete plugin-sdk CPed/model/renderer calls in this translation unit.
// Different plugin-sdk revisions expose slightly different constructors and
// helpers, so the project intentionally does not hard-code unverified engine
// addresses or pretend a CPed constructor is portable across revisions.
//
// Required implementation sequence:
// 1. request/load model `skin` through CStreaming;
// 2. create a non-player CPed using the installed plugin-sdk revision;
// 3. add it to CWorld;
// 4. Apply(): set matrix position/heading + health on gameProcessEvent;
// 5. DrawNameTag(): project ped head position using the SDK renderer;
// 6. Despawn(): remove from CWorld and delete/release the entity.
//
// This boundary is deliberately compile-safe until those calls are mapped to
// the exact plugin-sdk checkout selected by the builder.
#endif
