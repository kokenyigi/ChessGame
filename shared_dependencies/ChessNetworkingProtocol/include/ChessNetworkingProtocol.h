#pragma once
#ifndef CHESS_NETWORKING_PROTOCOL_H
#define CHESS_NETWORKING_PROTOCOL_H

#include <cstdint>

#include "ChessGame.h"

/**
 * This enumeration is the first byte of every single packet sent through
 *  the network.
 * These and only these are the packet types the CNP(Chess networking protocol)
 *  communicates through. The server receives these kinds of packages and
 *  returns the same kind.
 */
enum ChessNetworkPacketType : uint8_t
{
    PACKET_UNDEFINED = 0u, // for packets without a type 
    PACKET_CONNECTION_LOST, // when the server closes the connection for some reason(could be alright, could be a crash)
    PACKET_ENTER_GAME_QUEUE,
    PACKET_GAME_INIT,  // for when a game starts, some information ahs to reach each client's machine(like playing color)
    PACKET_GAME_ACTION, // used for draw_request and resign, for when a 
    PACKET_GAME_MOVE, // used for moves, like from, to , special_flags
};

/////////////////////////////////////////////////////////////////

/**
 * The chess networking initialization packet is sent to clients from the server
 *  when a game begins, to notify the palyers who is who(color)
 */
struct ChessNetworkInitPacket
{
    ChessNetworkPacketType type = PACKET_GAME_INIT;
    ChessColorType playerStartingColor = COLOR_LIGHT;
};

/////////////////////////////////////////////////////////////////

// actions a client or server can send 
enum ChessGameActionType : uint8_t
{
    NETWORK_ACTION_DRAW_REQUEST,
    NETWORK_ACTION_RESIGN
};

/**
 * The networking action packet holds different messages sent by server or client.
 */
struct ChessNetworkGameActionPacket
{
    ChessNetworkPacketType type = PACKET_GAME_ACTION;
    ChessGameActionType actionType = NETWORK_ACTION_DRAW_REQUEST;
};

/////////////////////////////////////////////////////////////////

/**
 * The networking move packet is sent when a move is made. has extra information storage as in promotionPiece when a promotion also happened.
 * If a move and a promotion is made, this packet should only be sent when the promotion concludes.(since thats the actual end of the move)
 */
struct ChessNetworkGameMovePacket
{
    ChessNetworkPacketType type = PACKET_GAME_MOVE;
    uint8_t from = 64u;
    uint8_t to = 64u;
    ChessPieceType promotionPiece = PIECE_NONE;
};

/////////////////////////////////////////////////////////////////

union ChessNetworkPacketUnion
{
    ChessNetworkPacketType type;
    ChessNetworkInitPacket gameInit;
    ChessNetworkGameActionPacket gameAction;
    ChessNetworkGameMovePacket gameMove;
    
};



#endif