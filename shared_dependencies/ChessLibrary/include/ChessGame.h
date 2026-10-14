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

enum class ChessMoveResultType: uint8_t
{
    RESULT_FAILURE = 0u,
    RESULT_SUCCESS = 1u
};

enum class ChessGameStateType: uint8_t
{
    STATE_PLAYING = 0u,
    STATE_PROMOTION_REQUIRED = 1u,
    STATE_DRAW_REQUESTABLE = 2u,
    STATE_DRAW_REPETITION = 3u,
    STATE_DRAW_STALEMATE = 4u,
    STATE_DRAW_INSUFFICIENT_MATERIAL = 5u,
    STATE_GAME_WON = 6u,
};

struct ChessMoveResultData
{
    ChessMoveResultType type = ChessMoveResultType::RESULT_FAILURE;
    ChessGameStateType gameState = ChessGameStateType::STATE_PLAYING;
};

struct ChessLegalMovesData
{
    uint64_t moveBitmask = 0ull;
    uint64_t captureBitmask = 0ull;
};

struct ChessTileData
{
    ChessPieceType piece = PIECE_NONE;
    ChessColorType color = COLOR_NONE;
};

class ChessGame
{
private:

    ChessGameState _state;

    ChessGameStateType _gameStateType = ChessGameStateType::STATE_PLAYING;

    std::vector<uint64_t> _previousZobristHashes;

    ChessTileViewData _boardView[64];

public:
    ChessGame();

    /**
     * This fucntion is used to resart the gamestate of a chessgame object, often used when a new match start(obviously)
     */
    void StartNewGame() {SetupStartState(); SetupViewBoardBasedOnState();}

    /**
     * This function is the core public function of this chessgame class.
     * In its very nature it tries to make the move specified by from and to, this happens through checking numerous chess-rules.
     * If this move is in any way shape or form invalid, it will return failure as its mainresult, otherwise success.
     * The subResult variable in its return value tells us what kind of state changed happends because of that move.
     *  - none indicates: nothing special happend
     *  - any game ending subresult state means well, that the game ended after that move.
     *  - a special subresult the func can return with: color_promoted -> this means that the move played was a pawn promotion, and
     *    the chess game class awaits the calling of another function: ChoosePromotionPiece to continue, it doesnt fully "complete" the
     *    given players move until this, other function is called.
     */
    ChessMoveResultData TryMove(unsigned int fromIndex, unsigned int toIndex);

    /**
     * This function chooses a piece(not a pawn or king) that will replace a promoted pawn.
     * Fails, if no promotion piece choosing is required, and returns success if a piece was succesfully chosen.
     * calling this function in the proper state also finishes the given player's turn, and therefore it calls a state checking, and
     * therefore it returns with the same state information subresult as the trymove function.
     */
    ChessMoveResultData ChoosePromotionPiece(ChessPieceType promotionPiece);

    /**
     * This function returns all the legal moves a given piece can make, where
     * index ->specifies the where the piece is located
     * Using complex chess-rules and state it calculates the valid bitmask, holding a 1 where the piece can move, and 0 everywhere else.
     */
    ChessLegalMovesData GetLegalMovesBitmaskOfPieceAt(uint8_t index);



    //Getter functions
    ChessTileViewData GetTileViewData(const unsigned int index);
    inline ChessColorType GetWhoseTurnItIs(){return (ChessColorType)this->_state.currentPlayer;}
    inline ChessColorType GetOpponentsColor(){return GetInverseColor((ChessColorType)this->_state.currentPlayer);}
    inline ChessGameStateType GetGameStateType(){return this->_gameStateType;}

    static uint64_t CreateBitmaskFromIndex(uint8_t index);
    static uint8_t GetLeastSignificantBitIndexFromBitmask(uint64_t bitmask);
    static uint64_t RemoveLeastSignificantBitOfBitmask(uint64_t bitmask);
private:
    /**
     * This method setups a clean, new chessboard state, with the valid rules and all.
     * Used when a newgame is created.
     */
    void SetupStartState();
    void SetupViewBoardBasedOnState();

    
    void SwapToOtherPlayer(){_state.currentPlayer = GetOpponentsColor();}

    /**
     * This function returns all positions a given color occupies in a bitmask
     */
    uint64_t GetPiecePositionsBitmaskOf(ChessColorType color);

    /**
     * This function uses the inner _state to get what tile information is on a given index
     * (might be slow)
     */
    ChessTileData GetTileDataAt(uint8_t index);

    inline ChessColorType GetInverseColor(ChessColorType color) {return color == COLOR_LIGHT ? COLOR_DARK :COLOR_LIGHT;}

    /**
     * This function i used when we want to get the logical, chess-rule safe moves of a given piece, this doesnt check legality yet.
     */
    uint64_t GetPseudoLegalMovesOfPiece(uint8_t index, ChessPieceType piece, ChessColorType color);

    uint64_t GetPseudoLegalMovesOfPawn(uint8_t index, ChessColorType color);
    uint64_t GetPseudoLegalMovesOfBishop(uint8_t index, ChessColorType color);
    uint64_t GetPseudoLegalMovesOfHorse(uint8_t index, ChessColorType color);
    uint64_t GetPseudoLegalMovesOfRook(uint8_t index, ChessColorType color);
    uint64_t GetPseudoLegalMovesOfQueen(uint8_t index, ChessColorType color);
    uint64_t GetPseudoLegalMovesOfKing(uint8_t index, ChessColorType color);


    /**
     * This function calculates a given color's attack bitmask, that is, all positions where if the opposite color's king is in, then
     * the king would be attacked.
     */
    uint64_t GetAttackBitmaskOf(ChessColorType color);

    uint64_t GetAttackBitmaskOfPiece(uint8_t index,ChessPieceType piece, ChessColorType color);

    uint64_t GetAttackBitmaskOfPawn(uint8_t index, ChessColorType color);
    uint64_t GetAttackBitmaskOfBishop(uint8_t index, ChessColorType color);
    uint64_t GetAttackBitmaskOfHorse(uint8_t index, ChessColorType color);
    uint64_t GetAttackBitmaskOfRook(uint8_t index, ChessColorType color);
    uint64_t GetAttackBitmaskOfQueen(uint8_t index, ChessColorType color);
    uint64_t GetAttackBitmaskOfKing(uint8_t index, ChessColorType color);

    void DeletePieceAt(uint8_t index, ChessPieceType piece, ChessColorType color);
    void PutPieceAt(uint8_t index, ChessPieceType piece, ChessColorType color);

    void ApplyMove(uint8_t from, uint8_t to);

    /**
     * This function is responsible for finalizing the game state after the given move, or promotion is concluded.
     * This function is only called once per full move, meaning it is called after promotion called XOR non-promotion move, 
     *   never in a promotion move.
     * This move returns the state the game gets into after the full move is finished.
     */
   ChessGameStateType FinalizeStateAfterMove();

   bool HasAnyLegalMoves(ChessColorType color);
};

#endif