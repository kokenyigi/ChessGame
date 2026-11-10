#include "ChessNetworker.h"

void ChessNetworker::Init()
{
    // if we are on windows, we have to initialize wsa stuff, and destroy it later too.
#if defined(_WIN32)
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // shouldn't fail, but could.
#endif
}

bool ChessNetworker::ConnectToServer(const std::string &addressIp,unsigned int addressPort)
{
    if(this->_isConnectedToServer == true) return false; // if already connected we cant connect again.

    int connectionResultOfSocket = this->_socket.Connect(addressIp.c_str(),addressPort);
    if(connectionResultOfSocket < 0)
    {
        return false;
    }

    // lot of other stuff (threads, stuff, and more)

    return true;
}

void ChessNetworker::Destory()
{
#if defined(_WIN32)
    int result = WSACleanup(); // shouldn't fail, but could.
#endif
}
