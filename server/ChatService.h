#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <algorithm>
namespace gtanet {
class ChatService {
    std::unordered_map<uint32_t,uint64_t> last_;
public:
    bool Accept(uint32_t player,uint64_t nowMs,const std::string& text){
        if(text.empty()||text.size()>160)return false;
        auto &t=last_[player]; if(t && nowMs-t<600)return false; t=nowMs;
        return std::none_of(text.begin(),text.end(),[](unsigned char c){return c<0x20 && c!='\t';});
    }
    void Remove(uint32_t p){last_.erase(p);}
};
}