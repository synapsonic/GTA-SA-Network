#pragma once
#include <cstdint>
#include <deque>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace gtanet {
struct HistorySample { uint64_t t=0; float x=0,y=0,z=0; };
struct Vec3 { float x=0,y=0,z=0; };
class LagCompensation {
    std::unordered_map<uint32_t,std::deque<HistorySample>> history_;
    static constexpr uint64_t KEEP_MS=1200;
public:
    void Record(uint32_t player,uint64_t serverMs,float x,float y,float z){
        auto &q=history_[player]; q.push_back({serverMs,x,y,z});
        while(!q.empty() && serverMs-q.front().t>KEEP_MS) q.pop_front();
    }
    bool Rewind(uint32_t player,uint64_t targetMs,Vec3& out) const {
        auto it=history_.find(player); if(it==history_.end()||it->second.empty()) return false;
        const auto &q=it->second;
        if(targetMs<=q.front().t){out={q.front().x,q.front().y,q.front().z};return true;}
        for(size_t i=1;i<q.size();++i) if(q[i].t>=targetMs){
            const auto&a=q[i-1];const auto&b=q[i];float f=b.t==a.t?0.f:float(targetMs-a.t)/float(b.t-a.t);
            out={a.x+(b.x-a.x)*f,a.y+(b.y-a.y)*f,a.z+(b.z-a.z)*f};return true;
        }
        out={q.back().x,q.back().y,q.back().z};return true;
    }
    void Remove(uint32_t player){history_.erase(player);}
    static uint64_t ClampRewind(uint64_t now,uint64_t requested){
        constexpr uint64_t MAX_REWIND=350;
        if(requested>now) return now;
        return now-requested>MAX_REWIND?now-MAX_REWIND:requested;
    }
    static bool RayHitsPlayer(Vec3 o,Vec3 d,Vec3 player,float maxRange,float radius=.45f,float halfHeight=.9f){
        float len=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z); if(len<0.0001f)return false;
        d={d.x/len,d.y/len,d.z/len};
        Vec3 c{player.x,player.y,player.z+halfHeight};
        float px=c.x-o.x,py=c.y-o.y,pz=c.z-o.z;
        float t=px*d.x+py*d.y+pz*d.z; if(t<0||t>maxRange)return false;
        Vec3 closest{o.x+d.x*t,o.y+d.y*t,o.z+d.z*t};
        float dx=closest.x-c.x,dy=closest.y-c.y;
        float horizontal=std::sqrt(dx*dx+dy*dy);
        float vertical=std::abs(closest.z-c.z);
        return horizontal<=radius && vertical<=halfHeight;
    }
};
}