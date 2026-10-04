#pragma once
#include "Resource.h"
#include <memory>
#include <vector>
namespace gtanet {
class ResourceManager {
    std::vector<std::unique_ptr<Resource>> resources_;
public:
    void Add(std::unique_ptr<Resource> r){resources_.push_back(std::move(r));}
    bool StartAll(){for(auto&i:resources_)if(!i->Start())return false;return true;}
    void Tick(uint64_t now){for(auto&i:resources_)i->Tick(now);}
    void StopAll(){for(auto i=resources_.rbegin();i!=resources_.rend();++i)(*i)->Stop();}
};
}