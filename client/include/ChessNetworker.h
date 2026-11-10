#pragma once
#ifndef CHESSNETWORKER_H
#define CHESSNETWORKER_H

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <iostream>

#include "Socket.h"
#include "RingBuffer.h"
#include "ChessNetworkingProtocol.h"

class ChessNetworker
{
private:

    bool _isConnectedToServer = false; // whenever the socket connects, this is true, otherwise false
    Socket _socket;

    std::thread _senderThread;
    std::condition_variable _senderThreadConditionVariable;
    std::mutex _senderQueueMutex;
    RingBuffer<ChessNetworkPacketUnion> _senderQueue;

    std::thread _receiverThread;
    std::condition_variable _receiverThreadConditionVariable;
    std::mutex _receiverQueueMutex;
    RingBuffer<ChessNetworkPacketUnion> _receiverQueue;

public:

    /**
     * This function creates the socket of the chessnetworker, and the necessary queues, zeros-them out, and gets ready for further
     *  workings.
     */
    void Init();

    /**
     * This function tries to connect to the remote host defined through its ip and port.
     * If it suceeds, spins up two threads, handling network traffic in such a way, that the main thread blocks as little as possible.
     * If everything worked out perfectly, return true , false if anything failed.
     */
    bool ConnectToServer(const std::string& addressIp,unsigned int addressPort);

    bool SendPacket(const ChessNetworkPacketUnion& packet);

    bool ProcessPackets();

    /**
     * This function tries to disconnect the socket from the server, if it can, it also begins to make a total teardown of the underlying networking
     *  threads.
     */
    bool DisconnectFromServer(); 


    /**
     * Cleans up different memory realted objects, on windows calls wsa cleanup fn
     */
    void Destory();


    inline bool GetIsConnectedToServer() {return _isConnectedToServer;}

private:


};


#endif