#pragma once
#include <deque>
#include <string>
#include <unordered_map>
#include <cstdint>
namespace gtanet {
struct ChatLine { std::string text; uint64_t timeMs=0; bool system=false; };
struct ScoreRow { uint32_t id=0; std::string name; uint16_t ping=0; int32_t score=0; };
class SocialUi {
    std::deque<ChatLine> chat_;
    std::unordered_map<uint32_t,ScoreRow> rows_;
public:
    void AddChat(std::string s,uint64_t t,bool sys=false){chat_.push_back({std::move(s),t,sys});while(chat_.size()>12)chat_.pop_front();}
    void Score(uint32_t id,std::string n,uint16_t p,int32_t score){rows_[id]={id,std::move(n),p,score};}
    void Remove(uint32_t id){rows_.erase(id);}
    const auto& Chat()const{return chat_;}
    const auto& Rows()const{return rows_;}
};
}