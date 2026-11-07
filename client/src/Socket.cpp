/**
 * This implementation is platform dependent
 */

 #include "Socket.h"

 #if defined(_WIN32)
    #include <ws2tcpip.h>
 #endif

 #include <iostream>

void Socket::Init()
{
    this->_crossPlatformSocket = socket(AF_INET,SOCK_STREAM,0);
    if(this->_crossPlatformSocket == INVALID_SOCKET)
    {
        std::cout<<"[Error]: Socket creation failed.\n";
    }
}

void Socket::Connect(const char *ipAddress, unsigned int port)
{
    struct sockaddr_in serverAdressData;
    memset(&serverAdressData,0,sizeof(struct sockaddr_in));

    serverAdressData.sin_family = AF_INET;
    serverAdressData.sin_port = htons(port);
    inet_pton(AF_INET,ipAddress,&serverAdressData.sin_addr);

    int connectionResult = connect(this->_crossPlatformSocket,(struct sockaddr*)&serverAdressData,sizeof(serverAdressData));
    if(connectionResult < 0)
    {
        std::cout<<"[Error]: Connection to the remote host failed.\n";
    }
}

// size should be between 0 and 2billion(int limit)
ssize_t Socket::Send(const char *buffer, size_t size)
{
    return send(this->_crossPlatformSocket,buffer,size,0);
}

// size should be between 0 and 2billion(int limit
ssize_t Socket::Recv(char *buffer, size_t size)
{
    ssize_t receiveResult = recv(this->_crossPlatformSocket,buffer,size,0);

    if(receiveResult >= 0)
    {
        return receiveResult;
    }

    // if we here -> result  == 0 or -1
    int errorCode = GetSocketError();
    if(errorCode == ERR_INTR) return 0;

    if(errorCode == ERR_AGAIN || errorCode == ERR_WBLOCK) return 0;

    //everything else: -> crash
    return -1;

}

void Socket::Shutdown()
{
    int shutdownResult = shutdown(this->_crossPlatformSocket,SHUT_RDWR);
    if(shutdownResult < 0)
    {
        std::cout<<"[Error]: Socket shutdown failed.\n";
    }
}

void Socket::Close()
{
    int closingResult = closesocket(this->_crossPlatformSocket);
    if(closingResult < 0)
    {
        std::cout<<"[Error]: Socket closure failed.\n";
    }
}
