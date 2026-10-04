#pragma once
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
namespace gtanet {
class Logger {
 std::mutex m_; std::ofstream file_;
public:
 explicit Logger(const std::string& path="server.log"):file_(path,std::ios::app){}
 void Write(const char* level,const std::string& msg){
  std::lock_guard<std::mutex> l(m_);
  auto now=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::string line="["+std::string(level)+"] "+std::to_string((long long)now)+" "+msg;
  std::cout<<line<<"\n"; if(file_)file_<<line<<"\n";
 }
 void Info(const std::string&m){Write("INFO",m);} void Warn(const std::string&m){Write("WARN",m);} void Error(const std::string&m){Write("ERROR",m);}
};
}