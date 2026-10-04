#pragma once
#include <fstream>
#include <mutex>
#include <string>
namespace gtanet {
class ClientLog {
 std::mutex m_;std::ofstream f_;
public:
 static ClientLog& Instance(){static ClientLog x;return x;}
 void Open(const std::string& path="gtanet-client.log"){std::lock_guard<std::mutex>l(m_);if(!f_.is_open())f_.open(path,std::ios::app);}
 void Write(const std::string& s){std::lock_guard<std::mutex>l(m_);if(f_)f_<<s<<"\n"<<std::flush;}
};
#define GTANET_CLIENT_LOG(x) do{::gtanet::ClientLog::Instance().Open();::gtanet::ClientLog::Instance().Write(x);}while(0)
}