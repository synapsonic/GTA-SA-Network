#pragma once
#include <cstdint>
#include <cstring>

namespace gtanet {
constexpr uint32_t MAGIC = 0x4754414E; // GTAN
constexpr uint16_t VERSION = 12;
constexpr std::size_t NAME_LEN = 24;

enum class PacketType : uint16_t {
    Hello = 1, Welcome = 2, PlayerJoin = 3, PlayerLeave = 4,
    PlayerState = 5, Chat = 6, Ping = 7, Pong = 8,
    VehicleSpawn = 9, VehicleState = 10, VehicleDestroy = 11,
    VehicleEnter = 12, VehicleExit = 13,
    VehicleClaim = 14, VehicleOwnership = 15,
    AimState = 16, Shot = 17, DamageRequest = 18, PlayerHealth = 19,
    PlayerDeath = 20, PlayerRespawn = 21, KillFeed = 22,
    ChatMessage = 23, Command = 24, ServerInfo = 25,
    ScoreboardRequest = 26, ScoreboardEntry = 27, SystemMessage = 28,
    AuthRequest = 29, AuthResult = 30, AdminAction = 31
};

#pragma pack(push,1)
struct PacketHeader { uint32_t magic=MAGIC; uint16_t version=VERSION; PacketType type{}; uint16_t size=0; };
struct HelloPacket { PacketHeader h{MAGIC,VERSION,PacketType::Hello,sizeof(HelloPacket)}; char name[NAME_LEN]{}; uint16_t skin=0; };
struct WelcomePacket { PacketHeader h{MAGIC,VERSION,PacketType::Welcome,sizeof(WelcomePacket)}; uint32_t playerId=0; };
struct PlayerJoinPacket { PacketHeader h{MAGIC,VERSION,PacketType::PlayerJoin,sizeof(PlayerJoinPacket)}; uint32_t playerId=0; char name[NAME_LEN]{}; uint16_t skin=0; };
struct PlayerLeavePacket { PacketHeader h{MAGIC,VERSION,PacketType::PlayerLeave,sizeof(PlayerLeavePacket)}; uint32_t playerId=0; };
struct PlayerStatePacket { PacketHeader h{MAGIC,VERSION,PacketType::PlayerState,sizeof(PlayerStatePacket)}; uint32_t playerId=0; float x=0,y=0,z=0; float heading=0; float health=100; uint16_t animation=0; uint16_t animationFlags=0; uint8_t weapon=0; uint8_t weaponState=0; uint8_t flags=0; uint8_t movementState=0; uint32_t sequence=0; };

struct VehicleSpawnPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::VehicleSpawn,sizeof(VehicleSpawnPacket)};
    uint32_t vehicleId=0; uint32_t ownerPlayerId=0; uint16_t modelId=400;
    uint8_t color1=0, color2=0; float x=0,y=0,z=0,heading=0; float health=1000;
};
struct VehicleStatePacket {
    PacketHeader h{MAGIC,VERSION,PacketType::VehicleState,sizeof(VehicleStatePacket)};
    uint32_t vehicleId=0; uint32_t ownerPlayerId=0;
    float x=0,y=0,z=0; float heading=0;
    float vx=0,vy=0,vz=0; float health=1000;
    uint32_t driverPlayerId=0; uint32_t sequence=0;
    uint64_t serverTimeMs=0; uint8_t flags=0;
};
struct VehicleDestroyPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::VehicleDestroy,sizeof(VehicleDestroyPacket)};
    uint32_t vehicleId=0;
};
struct VehicleEnterPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::VehicleEnter,sizeof(VehicleEnterPacket)};
    uint32_t playerId=0; uint32_t vehicleId=0; int8_t seat=0;
};
struct VehicleExitPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::VehicleExit,sizeof(VehicleExitPacket)};
    uint32_t playerId=0; uint32_t vehicleId=0;
};
struct VehicleClaimPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::VehicleClaim,sizeof(VehicleClaimPacket)};
    uint32_t playerId=0; uint32_t vehicleId=0; int8_t seat=0;
};
struct VehicleOwnershipPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::VehicleOwnership,sizeof(VehicleOwnershipPacket)};
    uint32_t vehicleId=0; uint32_t ownerPlayerId=0; uint32_t revision=0;
};

struct AimStatePacket {
    PacketHeader h{MAGIC,VERSION,PacketType::AimState,sizeof(AimStatePacket)};
    uint32_t playerId=0; float ox=0,oy=0,oz=0; float dx=0,dy=0,dz=0;
    uint8_t weapon=0; uint8_t aiming=0; uint32_t sequence=0;
};
struct ShotPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::Shot,sizeof(ShotPacket)};
    uint32_t playerId=0; uint32_t shotId=0; uint8_t weapon=0;
    float ox=0,oy=0,oz=0; float dx=0,dy=0,dz=0; uint64_t clientTimeMs=0;
};
struct DamageRequestPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::DamageRequest,sizeof(DamageRequestPacket)};
    uint32_t attackerId=0; uint32_t victimId=0; uint32_t shotId=0;
    uint8_t weapon=0; float damage=0; float hitX=0,hitY=0,hitZ=0;
};
struct PlayerHealthPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::PlayerHealth,sizeof(PlayerHealthPacket)};
    uint32_t playerId=0; float health=100; float armour=0;
};
struct PlayerDeathPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::PlayerDeath,sizeof(PlayerDeathPacket)};
    uint32_t victimId=0; uint32_t killerId=0; uint8_t weapon=0;
};

struct KillFeedPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::KillFeed,sizeof(KillFeedPacket)};
    uint32_t killerId=0; uint32_t victimId=0; uint8_t weapon=0; uint8_t flags=0;
};

struct PlayerRespawnPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::PlayerRespawn,sizeof(PlayerRespawnPacket)};
    uint32_t playerId=0; float x=0,y=0,z=0; float heading=0; float health=100; float armour=0;
};


constexpr std::size_t CHAT_LEN = 160;
constexpr std::size_t SERVER_NAME_LEN = 48;
struct ChatMessagePacket {
    PacketHeader h{MAGIC,VERSION,PacketType::ChatMessage,sizeof(ChatMessagePacket)};
    uint32_t playerId=0; char text[CHAT_LEN]{};
};
struct CommandPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::Command,sizeof(CommandPacket)};
    uint32_t playerId=0; char text[CHAT_LEN]{};
};
struct SystemMessagePacket {
    PacketHeader h{MAGIC,VERSION,PacketType::SystemMessage,sizeof(SystemMessagePacket)};
    char text[CHAT_LEN]{};
};
struct ServerInfoPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::ServerInfo,sizeof(ServerInfoPacket)};
    char name[SERVER_NAME_LEN]{}; uint16_t maxPlayers=32; uint16_t onlinePlayers=0; uint8_t passworded=0;
};
struct ScoreboardRequestPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::ScoreboardRequest,sizeof(ScoreboardRequestPacket)};
};
struct ScoreboardEntryPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::ScoreboardEntry,sizeof(ScoreboardEntryPacket)};
    uint32_t playerId=0; char name[NAME_LEN]{}; uint16_t ping=0; int32_t score=0;
};


constexpr std::size_t AUTH_USER_LEN = 32;
constexpr std::size_t AUTH_SECRET_LEN = 96;
struct AuthRequestPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::AuthRequest,sizeof(AuthRequestPacket)};
    char username[AUTH_USER_LEN]{}; char proof[AUTH_SECRET_LEN]{};
};
struct AuthResultPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::AuthResult,sizeof(AuthResultPacket)};
    uint8_t success=0; uint8_t role=0; uint32_t accountId=0; char message[CHAT_LEN]{};
};
enum class AdminActionType : uint8_t { Kick=1, Ban=2, Unban=3 };
struct AdminActionPacket {
    PacketHeader h{MAGIC,VERSION,PacketType::AdminAction,sizeof(AdminActionPacket)};
    AdminActionType action=AdminActionType::Kick; uint32_t targetPlayerId=0; uint32_t durationSeconds=0;
    char reason[CHAT_LEN]{};
};

struct PingPacket { PacketHeader h{MAGIC,VERSION,PacketType::Ping,sizeof(PingPacket)}; uint64_t tick=0; };
struct PongPacket { PacketHeader h{MAGIC,VERSION,PacketType::Pong,sizeof(PongPacket)}; uint64_t tick=0; };
#pragma pack(pop)

inline bool ValidHeader(const PacketHeader& h) { return h.magic==MAGIC && h.version==VERSION && h.size>=sizeof(PacketHeader) && h.size<=4096; }
}
