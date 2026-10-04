#pragma once
#include <deque>
#include <cstdint>
#include <algorithm>
#include "../shared/Protocol.h"
namespace gtanet {
class VehicleSnapshotBuffer {
    std::deque<VehicleStatePacket> q_;
public:
    void Push(const VehicleStatePacket& s) {
        if(!q_.empty() && s.sequence<=q_.back().sequence) return;
        q_.push_back(s); while(q_.size()>32) q_.pop_front();
    }
    bool Sample(uint64_t renderTimeMs, VehicleStatePacket& out) const {
        if(q_.empty()) return false;
        if(q_.size()==1){ out=q_.back(); return true; }
        const VehicleStatePacket *a=nullptr,*b=nullptr;
        for(size_t i=1;i<q_.size();++i) if(q_[i].serverTimeMs>=renderTimeMs){ a=&q_[i-1]; b=&q_[i]; break; }
        if(a&&b && b->serverTimeMs>a->serverTimeMs){
            float t=float(renderTimeMs-a->serverTimeMs)/float(b->serverTimeMs-a->serverTimeMs);
            t=std::clamp(t,0.0f,1.0f); out=*b;
            out.x=a->x+(b->x-a->x)*t; out.y=a->y+(b->y-a->y)*t; out.z=a->z+(b->z-a->z)*t;
            out.heading=a->heading+(b->heading-a->heading)*t;
            out.vx=a->vx+(b->vx-a->vx)*t; out.vy=a->vy+(b->vy-a->vy)*t; out.vz=a->vz+(b->vz-a->vz)*t;
            out.health=a->health+(b->health-a->health)*t; return true;
        }
        out=q_.back();
        if(renderTimeMs>out.serverTimeMs){
            uint64_t dt=std::min<uint64_t>(renderTimeMs-out.serverTimeMs,150);
            float sec=float(dt)/1000.0f;
            out.x+=out.vx*sec; out.y+=out.vy*sec; out.z+=out.vz*sec;
        }
        return true;
    }
};
}