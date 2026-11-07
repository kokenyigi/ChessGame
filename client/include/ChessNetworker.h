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

class ChessNetworker
{
private:

    std::thread _senderThread;
    RingBuffer<> _senderQueue;

    std::thread _receiverThread;
    RingBuffer<> _recieverQueue;

public:

private:


};


#endif