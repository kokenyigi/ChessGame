#include "ChessGame.h"

ChessGame::ChessGame()
{
    StartNewGame();
}

ChessTileViewData ChessGame::GetTileViewData(const ChessPositionData position)
{
    if(position.file < 0 || position.file >= 8  || position.rank < 0 || position.file >= 8) return {PIECE_NONE, COLOR_NONE};

    uint8_t indexFromPosition = GetIndexFromPosition(position);
    return this->_boardView[indexFromPosition];
}

void ChessGame::SetupStartState()
{
    for(int color = 0 ; color < 2 ; ++color)
    {
        for(int piece = 0 ; piece < 6 ; ++ piece)
        {
            this->_state.piecePositions[color][piece] = 0u;
        }
    }

    //position setup
    
    //pawns
    uint64_t& lightPawnsPosRef = this->_state.piecePositions[COLOR_LIGHT][PIECE_PAWN];
    uint64_t& darkPawnsPosRef = this->_state.piecePositions[COLOR_DARK][PIECE_PAWN];
    for(unsigned int file=0;file<8; ++file)
    {
        unsigned int lightCurrentPawnIndex = 8u + file;
        lightPawnsPosRef |= CreateBitmaskFromIndex(lightCurrentPawnIndex); // we "put" down the pawns with this bitwise - OR 

        unsigned int darkCurrentPawnIndex = 48u + file;
        darkPawnsPosRef |= CreateBitmaskFromIndex(darkCurrentPawnIndex);
    }

    // rooks
    this->_state.piecePositions[COLOR_LIGHT][PIECE_ROOK] |= CreateBitmaskFromIndex(0u);
    this->_state.piecePositions[COLOR_LIGHT][PIECE_ROOK] |= CreateBitmaskFromIndex(7u);

    this->_state.piecePositions[COLOR_DARK][PIECE_ROOK] |= CreateBitmaskFromIndex(56u);
    this->_state.piecePositions[COLOR_DARK][PIECE_ROOK] |= CreateBitmaskFromIndex(63u);

    //horses
    this->_state.piecePositions[COLOR_LIGHT][PIECE_ROOK] |= CreateBitmaskFromIndex(1u);
    this->_state.piecePositions[COLOR_LIGHT][PIECE_ROOK] |= CreateBitmaskFromIndex(6u);

    this->_state.piecePositions[COLOR_DARK][PIECE_ROOK] |= CreateBitmaskFromIndex(57u);
    this->_state.piecePositions[COLOR_DARK][PIECE_ROOK] |= CreateBitmaskFromIndex(62u);


    //bishops
    this->_state.piecePositions[COLOR_LIGHT][PIECE_BISHOP] |= CreateBitmaskFromIndex(2u);
    this->_state.piecePositions[COLOR_LIGHT][PIECE_BISHOP] |= CreateBitmaskFromIndex(5u);

    this->_state.piecePositions[COLOR_DARK][PIECE_BISHOP] |= CreateBitmaskFromIndex(58u);
    this->_state.piecePositions[COLOR_DARK][PIECE_BISHOP] |= CreateBitmaskFromIndex(61u);

    //queens
    this->_state.piecePositions[COLOR_LIGHT][PIECE_QUEEN] |= CreateBitmaskFromIndex(3u);
    this->_state.piecePositions[COLOR_DARK][PIECE_QUEEN] |= CreateBitmaskFromIndex(59u);

    //kings
    this->_state.piecePositions[COLOR_LIGHT][PIECE_KING] |= CreateBitmaskFromIndex(4u);
    this->_state.piecePositions[COLOR_DARK][PIECE_KING] |= CreateBitmaskFromIndex(60u);

    //white is always the first to move
    this->_state.currentPlayer = COLOR_LIGHT;

    // in the beginning state every castling right is provided
    this->_state.castlingRights = CASTLING_BLACK_KINGSIDE | CASTLING_BLACK_QUEENSIDE | CASTLING_WHITE_KINGSIDE | CASTLING_WHITE_QUEENSIDE;

    // at first, no enpassant file is provided obviously.
    this->_state.enPassantFile = FILE_NONE;

    // we have to reset the halfmove counter(the counter which counts how many moves have been made since the last capture or pawn move)
    this->_state.halfMoveCount = 0u;

    //We have set the positions for the pieces
    // we have to clear the previously stored zobrist hashes
    this->_previousZobristHashes.clear();
    
}

void ChessGame::SetupViewBoardBasedOnState()
{
    for(unsigned int color=0;color<2;++color)
    {
        for(unsigned int piece = 0; piece < 6 ;++piece)
        {
            uint64_t currentPieceMaskCopy = this->_state.piecePositions[color][piece];

            unsigned int indexCounter = 0;
            while(currentPieceMaskCopy > 0)
            {
                if(currentPieceMaskCopy & (uint64_t)1ul) // if there is a one on its least important bit
                {
                    this->_boardView[indexCounter].piece = (ChessPieceType)piece;
                    this->_boardView[indexCounter].color = (ChessColorType)color;
                }

                currentPieceMaskCopy >>= 1u;
                ++indexCounter;
            }

        }
    }
}
