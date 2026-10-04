#pragma once
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using SocketHandle=SOCKET; constexpr SocketHandle INVALID_NET_SOCKET=INVALID_SOCKET;
inline bool NetInit(){WSADATA w{};return WSAStartup(MAKEWORD(2,2),&w)==0;} inline void NetCleanup(){WSACleanup();}
inline void CloseSocket(SocketHandle s){closesocket(s);} inline void ShutdownSocket(SocketHandle s){shutdown(s,SD_BOTH);}
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using SocketHandle=int; constexpr SocketHandle INVALID_NET_SOCKET=-1;
inline bool NetInit(){return true;} inline void NetCleanup(){} inline void CloseSocket(SocketHandle s){close(s);} inline void ShutdownSocket(SocketHandle s){shutdown(s,SHUT_RDWR);}
#endif
