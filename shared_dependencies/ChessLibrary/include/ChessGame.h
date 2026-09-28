#pragma once
#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <cstdint>
#include <vector>

enum ChessPieceType : uint8_t
{
    PIECE_PAWN = 0u,
    PIECE_BISHOP = 1u,
    PIECE_HORSE = 2u,
    PIECE_ROOK = 3u,
    PIECE_QUEEN = 4u,
    PIECE_KING = 5u,
    PIECE_NONE = 6u
};

enum ChessColorType: uint8_t
{
    COLOR_LIGHT = 0u,
    COLOR_DARK = 1u,
    COLOR_NONE = 2u
};

enum ChessCastlingRightBitmasks: uint8_t
{
    CASTLING_WHITE_KINGSIDE = (1u << 0u),
    CASTLING_WHITE_QUEENSIDE = (1u << 1u),
    CASTLING_BLACK_KINGSIDE = (1u << 2u),
    CASTLING_BLACK_QUEENSIDE = (1u << 3u),
};

enum ChessEnpassantFileType: uint8_t
{
    FILE_A = 0u,
    FILE_B = 1u,
    FILE_C = 2u,
    FILE_D = 3U,
    FILE_E = 4U,
    FILE_F = 5U,
    FILE_G = 6U,
    FILE_H = 7U,
    FILE_NONE = 8u
};

struct ChessGameState
{
    uint64_t piecePositions[2][6];
    uint8_t currentPlayer = COLOR_LIGHT;
    uint8_t castlingRights = 0u; // bits
    uint8_t enPassantFile = FILE_NONE;
    uint8_t halfMoveCount = 0u;
};

struct ChessTileViewData
{
    ChessPieceType piece = PIECE_NONE;
    ChessColorType color = COLOR_NONE;
};

struct ChessPositionData
{
    uint8_t file = 0u;
    uint8_t rank = 0u;
};

class ChessGame
{
private:

    ChessGameState _state;

    std::vector<uint64_t> _previousZobristHashes;

    ChessTileViewData _boardView[64];

public:
    ChessGame();

    void StartNewGame() {SetupStartState(); SetupViewBoardBasedOnState();}


    static uint8_t GetIndexFromPosition(const ChessPositionData position);
    static ChessPositionData GetPositionFromIndex(const uint8_t index);
    static uint64_t CreateBitmaskFromIndex(const uint8_t index){return (1ul << index);}

    ChessTileViewData GetTileViewData(const ChessPositionData position);

private:
    void SetupStartState();

    void SetupViewBoardBasedOnState();

};

#endif