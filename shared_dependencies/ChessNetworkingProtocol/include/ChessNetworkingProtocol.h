#pragma once
#ifndef CHESS_NETWORKING_PROTOCOL_H
#define CHESS_NETWORKING_PROTOCOL_H

#include <cstdint>

enum ChessNetworkPacketType : uint8_t
{
    PACKET_UNDEFINED = 0u, // for packets without a type 
    PACKET_CONNECTION_LOST, // when the server closes the connection for some reason(could be alright, could be a crash)
    PACKET_ENTER_GAME_QUEUE,
    PACKET_GAME_INIT,  // for when a game starts, some information ahs to reach each client's machine(like playing color)
    PACKET_GAME_ACTION, // used for draw_request and resign, for when a 
    PACKET_GAME_MOVE, // used for moves, like from, to , special_flags
};




#endif