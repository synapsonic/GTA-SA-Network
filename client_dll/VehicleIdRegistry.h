#pragma once
#include <cstdint>
#include <unordered_map>
#include <mutex>
namespace gtanet {
class VehicleIdRegistry {
 std::mutex m_;std::unordered_map<void*,uint32_t> byHandle_;std::unordered_map<uint32_t,void*> byId_;
public:
 static VehicleIdRegistry& Instance(){static VehicleIdRegistry r;return r;}
 void Bind(uint32_t id,void*h){if(!id||!h)return;std::lock_guard<std::mutex>l(m_);byHandle_[h]=id;byId_[id]=h;}
 void Unbind(uint32_t id){std::lock_guard<std::mutex>l(m_);auto i=byId_.find(id);if(i!=byId_.end()){byHandle_.erase(i->second);byId_.erase(i);}}
 uint32_t Id(void*h){std::lock_guard<std::mutex>l(m_);auto i=byHandle_.find(h);return i==byHandle_.end()?0:i->second;}
 void* Handle(uint32_t id){std::lock_guard<std::mutex>l(m_);auto i=byId_.find(id);return i==byId_.end()?nullptr:i->second;}
};
}