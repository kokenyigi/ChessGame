#pragma once 
#ifndef CROSS_PLATFORM_SOCKET_H
#define CROSS_PLATFORM_SOCKET_H

#include <cstdio>

#if defined(_WIN32)

    #include <winsock2.h>
    #include <stddef.h>
    typedef ptrdiff_t ssize_t;
    #pragma comment(lib, "Ws2_32.lib") //needed for linking
    #define SHUT_RD SD_RECEIVE
    #define SHUT_WR SD_SEND
    #define SHUT_RDWR SD_BOTH
    #define GetSocketError() WSAGetLastError()
    #define ERR_WBLOCK WSAEWOULDBLOCK
    #define ERR_AGAIN WSAEWOULDBLOCK // Windows-on nincs külön EAGAIN
    #define ERR_INTR WSAEINTR 

#elif defined(__linux__)

    #include <sys/socket.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <sys/unistd.h> 
    #include <arpa/inet.h>
    typedef int SOCKET;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
    #define GetSocketError() errno
    #define ERR_WBLOCK EWOULDBLOCK
    #define ERR_AGAIN EAGAIN
    #define ERR_INTR EINTR

#else 

    #error "Platform can't be recognized, please switch to linux or windows based systems."

#endif

class Socket
{
private:
    // for windows:
    //WSADATA _windowsSocketInfo; only needed once per startup

    // SOCKET is linux-win cross platform definition
    SOCKET _crossPlatformSocket;

public:
    Socket(){}
    ~Socket(){}

    // Creates the TCP, ipv4 socket
    void Init();

    // connects to a host via its ipv4 ip and port
    int Connect(const char* ipAddress,unsigned int port);

    ssize_t Send(const char* buffer, size_t size);

    ssize_t Recv(char* buffer, size_t size);

    void Shutdown();

    void Close();
};


#endif

