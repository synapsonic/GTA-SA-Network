#include <windows.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
int main(){std::cout<<"GTA SA Network Launcher v0.2\n";std::string game,host="127.0.0.1",port="7777",name="Player",t;std::cout<<"Path to gta_sa.exe: ";std::getline(std::cin,game);std::cout<<"Nickname [Player]: ";std::getline(std::cin,t);if(!t.empty())name=t;std::cout<<"Server host [127.0.0.1]: ";std::getline(std::cin,t);if(!t.empty())host=t;std::cout<<"Server port [7777]: ";std::getline(std::cin,t);if(!t.empty())port=t;if(!std::filesystem::exists(game)){std::cerr<<"gta_sa.exe not found\n";return 1;}auto dir=std::filesystem::path(game).parent_path();std::ofstream ini(dir/"network.ini");ini<<"[network]\nhost="<<host<<"\nport="<<port<<"\nname="<<name<<"\n";ini.close();STARTUPINFOA si{};si.cb=sizeof(si);PROCESS_INFORMATION pi{};std::string cmd='"'+game+'"';if(!CreateProcessA(nullptr,cmd.data(),nullptr,nullptr,FALSE,0,nullptr,dir.string().c_str(),&si,&pi)){std::cerr<<"Launch failed: "<<GetLastError()<<"\n";return 2;}CloseHandle(pi.hThread);CloseHandle(pi.hProcess);return 0;}
