#pragma once
#include "LuaResource.h"
#include "ResourceManifest.h"
#include "ScriptApi.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
namespace gtanet {
enum class ResourceState { Stopped, Starting, Running, Failed };
struct RuntimeResource {
    ResourceManifest manifest;
    std::filesystem::path directory;
    std::unique_ptr<LuaResource> instance;
    ResourceState state=ResourceState::Stopped;
    std::filesystem::file_time_type newestWrite{};
};
class ResourceRuntime {
    std::filesystem::path root_;
    ScriptApi api_;
    std::unordered_map<std::string,RuntimeResource> items_;
    std::filesystem::file_time_type NewestWrite(const std::filesystem::path&) const;
    bool DependenciesRunning(const RuntimeResource&) const;
public:
    ResourceRuntime(std::filesystem::path root,ScriptApi api):root_(std::move(root)),api_(std::move(api)){}
    size_t Discover();
    bool Start(const std::string& name);
    bool Stop(const std::string& name);
    bool Reload(const std::string& name);
    void Tick(uint64_t nowMs);
    void PollHotReload();
    ResourceState State(const std::string& name) const;
    std::vector<std::string> Names() const;
};
}