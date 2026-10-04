#pragma once
#include <cstdint>
#include <functional>
#include <unordered_map>
namespace gtanet {
struct ScriptTimer { uint32_t id=0; uint64_t due=0,interval=0; bool repeat=false; std::function<void()> callback; };
class ScriptTimers {
 uint32_t next_=1;std::unordered_map<uint32_t,ScriptTimer> timers_;
public:
 uint32_t Add(uint64_t now,uint64_t delay,bool repeat,std::function<void()> cb){uint32_t id=next_++;timers_[id]={id,now+delay,delay,repeat,std::move(cb)};return id;}
 void Cancel(uint32_t id){timers_.erase(id);}
 void Tick(uint64_t now){for(auto i=timers_.begin();i!=timers_.end();){if(i->second.due>now){++i;continue;}auto cb=i->second.callback;bool rep=i->second.repeat;auto interval=i->second.interval;if(rep)i->second.due=now+interval;else i=timers_.erase(i);if(cb)cb();if(rep)++i;}}
 void Clear(){timers_.clear();}
};
}