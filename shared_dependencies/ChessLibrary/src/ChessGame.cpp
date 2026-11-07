#include "ChessGame.h"

//#include "Utils.h"
#include <cmath>

#include <random>

ChessGame::ChessGame()
{
    this->_previousZobristHashes.reserve(151); // will never be higher than the halfmove counter

    SetupDefaultZobristKeys();

    StartNewGame();
}

ChessMoveResultData ChessGame::TryMove(unsigned int fromIndex, unsigned int toIndex)
{
    ChessMoveResultData retvalResult;
    if(_gameStateType != ChessGameStateType::STATE_PLAYING &&  _gameStateType != ChessGameStateType::STATE_DRAW_REQUESTABLE)
    {
        return retvalResult; // If we are in the wrong state, or the game has ended, we dont do anything.
    }

    if(fromIndex >= 64u || toIndex >= 64u || fromIndex == toIndex) return retvalResult;

    ChessTileData fromTileData = GetTileDataAt(fromIndex);
    ChessColorType currentPlayer = GetWhoseTurnItIs();
    ChessColorType enemyPlayer = GetInverseColor(currentPlayer);
    ChessTileData toTileData = GetTileDataAt(toIndex);

    uint8_t castlingRightsBefore = _state.castlingRights;

    if(fromTileData.color != currentPlayer || fromTileData.piece == PIECE_NONE) return retvalResult;

    uint64_t bitmaskOfPositionThePieceMovesTo = CreateBitmaskFromIndex(toIndex);
    uint64_t pseudoLegalMovesOfMovingPiece = GetPseudoLegalMovesOfPiece(fromIndex,fromTileData.piece, currentPlayer);
    if( (bitmaskOfPositionThePieceMovesTo & pseudoLegalMovesOfMovingPiece) == 0ull) return retvalResult;

    ChessGameState stateBeforeMove = _state;

    ApplyMove(fromIndex,toIndex);

    
    // if the king of the current player is attacked after the move
    uint64_t enemyAttackBitmask = GetAttackBitmaskOf(enemyPlayer);
    if((enemyAttackBitmask & this->_state.piecePositions[currentPlayer][PIECE_KING]) != 0ull) 
    {
        //our king is being attacked after the move, we can not play this move.
        this->_state = stateBeforeMove;

        return retvalResult;
    }
    retvalResult.type = ChessMoveResultType::RESULT_SUCCESS;

    
    if(fromTileData.piece == PIECE_PAWN || toTileData.piece != PIECE_NONE) // pawn move or capture
    {
        this->_halfMoveCount = 0u;
        
    }
    else
    {
        ++this->_halfMoveCount;
    }

    uint8_t castlingRightsAfter = _state.castlingRights;
    if(fromTileData.piece == PIECE_PAWN || toTileData.piece != PIECE_NONE || 
        (fromTileData.piece == PIECE_KING && castlingRightsAfter != castlingRightsBefore) )
    {
        this->_previousZobristHashes.clear();
    }

    SetupViewBoardBasedOnState(); // for view



    unsigned int toRank = toIndex / 8u;
    if(fromTileData.piece == PIECE_PAWN && (toRank == 7u || toRank == 0u) ) // if the move resulted in a pawn promotion
    {
        _gameStateType = ChessGameStateType::STATE_PROMOTION_REQUIRED;
        retvalResult.gameState = ChessGameStateType::STATE_PROMOTION_REQUIRED;
        return retvalResult;
    }

    SwapToOtherPlayer();

    _gameStateType = FinalizeStateAfterMove(); // we save the chessgamestatetype into our main state type
    retvalResult.gameState = _gameStateType; // we will also return it

    return retvalResult;
}

ChessMoveResultData ChessGame::ChoosePromotionPiece(ChessPieceType promotionPiece)
{
    ChessMoveResultData retvalResult;
    if(_gameStateType != ChessGameStateType::STATE_PROMOTION_REQUIRED) return retvalResult;

    if(promotionPiece < PIECE_BISHOP || promotionPiece > PIECE_QUEEN) return retvalResult;

    retvalResult.type = ChessMoveResultType::RESULT_SUCCESS;

    ChessColorType promotionColor = GetWhoseTurnItIs();
    uint64_t pawnsOfPromotingColor = this->_state.piecePositions[promotionColor][PIECE_PAWN];
    
    uint64_t finalRankBitmask = 0ull;
    if(promotionColor == COLOR_LIGHT)
    {
        finalRankBitmask =  0xFF00000000000000;
    }
    else // color dark
    {
        finalRankBitmask =  0x00000000000000FF;
    }

    //there is only one pawn at final rank of promoting color(ensured)
    uint64_t bitmaskOfPromotingPawn = finalRankBitmask & pawnsOfPromotingColor;
    //uint8_t indexOfPromotingPawn  = GetLeastSignificantBitIndexFromBitmask(bitmaskOfPromotingPawn); // there is only one bit

    //now we make the promotion "move"
    _state.piecePositions[promotionColor][PIECE_PAWN] &= ~bitmaskOfPromotingPawn; // remove pawn
    _state.piecePositions[promotionColor][promotionPiece] |= bitmaskOfPromotingPawn; // add promotion piece

    SetupViewBoardBasedOnState();


    SwapToOtherPlayer();


    _gameStateType = FinalizeStateAfterMove();
    retvalResult.gameState = _gameStateType;

    return retvalResult;
}

ChessLegalMovesData ChessGame::GetLegalMovesBitmaskOfPieceAt(uint8_t index)
{
    ChessLegalMovesData retvalLegalMoves;
    if(_gameStateType != ChessGameStateType::STATE_PLAYING &&  _gameStateType != ChessGameStateType::STATE_DRAW_REQUESTABLE)
    {
        return retvalLegalMoves; // we are in a game finished state, therefore we cant do anything further.
    }

    if(index >= 64u) return retvalLegalMoves;

    ChessTileData tileAtIndex = GetTileDataAt(index);
    if(tileAtIndex.piece == PIECE_NONE || tileAtIndex.color != (ChessColorType)_state.currentPlayer) return retvalLegalMoves;

    uint64_t pseudoLegalPositionsBitmaskOfPiece = GetPseudoLegalMovesOfPiece(index,tileAtIndex.piece,tileAtIndex.color);
    while(pseudoLegalPositionsBitmaskOfPiece)
    {
        uint8_t currentPseudoLegalPositionIndex = GetLeastSignificantBitIndexFromBitmask(pseudoLegalPositionsBitmaskOfPiece);

        ChessGameState stateBeforeMove = this->_state;
        uint64_t enemyPiecesBeforeMove = GetPiecePositionsBitmaskOf(GetInverseColor(tileAtIndex.color));

        ApplyMove(index,currentPseudoLegalPositionIndex);
        
        uint64_t currentEnemyAttackMask = GetAttackBitmaskOf(GetInverseColor(tileAtIndex.color));
        if( (currentEnemyAttackMask & _state.piecePositions[tileAtIndex.color][PIECE_KING]) == 0ull )
        {
            uint64_t enemyPiecesAfterMove = GetPiecePositionsBitmaskOf(GetInverseColor(tileAtIndex.color));
            if(enemyPiecesBeforeMove != enemyPiecesAfterMove) // a piece has been "captured"
            {
                retvalLegalMoves.captureBitmask |= CreateBitmaskFromIndex(currentPseudoLegalPositionIndex);
            }
            else
            {
                retvalLegalMoves.moveBitmask |= CreateBitmaskFromIndex(currentPseudoLegalPositionIndex);
            }
        }

        _state = stateBeforeMove;

        pseudoLegalPositionsBitmaskOfPiece = RemoveLeastSignificantBitOfBitmask(pseudoLegalPositionsBitmaskOfPiece);
    }

    return retvalLegalMoves;

}

ChessTileViewData ChessGame::GetTileViewData(const unsigned int index)
{
    if(index >= 64u) return {PIECE_NONE, COLOR_NONE};

    return this->_boardView[index];
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
    this->_state.piecePositions[COLOR_LIGHT][PIECE_HORSE] |= CreateBitmaskFromIndex(1u);
    this->_state.piecePositions[COLOR_LIGHT][PIECE_HORSE] |= CreateBitmaskFromIndex(6u);

    this->_state.piecePositions[COLOR_DARK][PIECE_HORSE] |= CreateBitmaskFromIndex(57u);
    this->_state.piecePositions[COLOR_DARK][PIECE_HORSE] |= CreateBitmaskFromIndex(62u);


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

    // in the beginning state every castling right is provided
    this->_state.castlingRights = CASTLING_BLACK_KINGSIDE | CASTLING_BLACK_QUEENSIDE | CASTLING_WHITE_KINGSIDE | CASTLING_WHITE_QUEENSIDE;

    // at first, no enpassant file is provided obviously.
    this->_state.enPassantFile = FILE_NONE;

    // we have to reset the halfmove counter(the counter which counts how many moves have been made since the last capture or pawn move)
    this->_halfMoveCount = 0u;

    //white is always the first to move
    this->_state.currentPlayer = COLOR_LIGHT;

    //We have set the positions for the pieces
    // we have to clear the previously stored zobrist hashes
    this->_previousZobristHashes.clear();
    this->_previousZobristHashes.push_back(this->CalculateZobristHashOfCurrentState());

    

    this->_gameStateType = ChessGameStateType::STATE_PLAYING;
    
}

void ChessGame::SetupViewBoardBasedOnState()
{
    for(int i=0;i<64;++i)
    {
        _boardView[i] = {PIECE_NONE,COLOR_NONE};
    }

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

uint64_t ChessGame::GetPiecePositionsBitmaskOf(ChessColorType color)
{
    if(color == COLOR_NONE) return 0ull;

    return _state.piecePositions[color][PIECE_PAWN] | _state.piecePositions[color][PIECE_BISHOP] | _state.piecePositions[color][PIECE_HORSE] |
        _state.piecePositions[color][PIECE_ROOK] | _state.piecePositions[color][PIECE_QUEEN] | _state.piecePositions[color][PIECE_KING];
}

ChessTileData ChessGame::GetTileDataAt(uint8_t index)
{
    if(index >= 64u) return {PIECE_NONE,COLOR_NONE};

    uint64_t bitmaskFromIndex = CreateBitmaskFromIndex(index);

    ChessTileData retval;
    if(bitmaskFromIndex & GetPiecePositionsBitmaskOf(COLOR_LIGHT)) // if the given piece is light
    {
        retval.color = COLOR_LIGHT;
    }
    else if(bitmaskFromIndex & GetPiecePositionsBitmaskOf(COLOR_DARK))
    {
        retval.color = COLOR_DARK;
    }
    else
    {
        return retval;
    }

    uint8_t pieceTypeIndex = 0u;
    while(pieceTypeIndex < 6u)
    {
        if(_state.piecePositions[retval.color][pieceTypeIndex] & bitmaskFromIndex)
        {
            retval.piece = (ChessPieceType)pieceTypeIndex;
            return retval;
        }

        ++pieceTypeIndex;
    }

    return {PIECE_NONE,COLOR_NONE};
}

uint64_t ChessGame::GetPseudoLegalMovesOfPiece(uint8_t index,ChessPieceType piece, ChessColorType color)
{
    switch(piece)
    {
        case PIECE_PAWN:
            return GetPseudoLegalMovesOfPawn(index, color);
        break;
        case PIECE_BISHOP:
            return GetPseudoLegalMovesOfBishop(index, color);
        break;
        case PIECE_HORSE:
            return GetPseudoLegalMovesOfHorse(index, color);
        break;
        case PIECE_ROOK:
            return GetPseudoLegalMovesOfRook(index, color);
        break;
        case PIECE_QUEEN:
            return GetPseudoLegalMovesOfQueen(index, color);
        break;
        case PIECE_KING:
            return GetPseudoLegalMovesOfKing(index, color);
        break;
    }
}

uint64_t ChessGame::GetPseudoLegalMovesOfPawn(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;

    uint64_t bitmaskOfPawn = CreateBitmaskFromIndex(index);
    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);

    ChessColorType enemyColor = GetInverseColor(color);
    uint64_t enemyPieces = GetPiecePositionsBitmaskOf(enemyColor);

    uint8_t rank = index / 8u;
    uint8_t file = index % 8u;
    if(color == COLOR_LIGHT)
    {
        if(rank >= 7u) return 0ull; // if our pawn is on the last rank, then it can go nowhere

        uint64_t forwardMoveMask = bitmaskOfPawn << 8u; // the position where the pawn can move with one step has this bitmask.
        if((forwardMoveMask & (friendlyPieces | enemyPieces)) == 0ull)
        {
            //this means that neither a friendly or an enemy piece is blocking the pawn
            //therefore the pawn can move forward, so we put it in our pseudo-legal moves return value
            retvalBitmask |= forwardMoveMask;
            
            //lets check for double move, which can only be done as the pawn's very first move.
            // we only do this check if the pawn can move forward atleast once
            if(rank == 1ull)
            {   
                uint64_t doubleForwardMoveMask = bitmaskOfPawn << 16u;
                if((doubleForwardMoveMask & (friendlyPieces | enemyPieces)) == 0ull)
                {
                    //can also double-move forward.
                    retvalBitmask |= doubleForwardMoveMask;
                }
            }
        }

        //lets check the sideways attacks now. (simple sideways attack move, and also en' passant !!)
        if(file > 0u) // if we are anywhere but the A file we can try checking our leftways sideattack.
        {
            uint64_t bitmaskOfLeftsideAttack = bitmaskOfPawn << 7u;
            if((bitmaskOfLeftsideAttack & enemyPieces) != 0ull) // if there is an enemy piece we accept the attack move
            {
                retvalBitmask |= bitmaskOfLeftsideAttack;
            }
            else if (rank == 4u)
            {
                // if we here: -> we are on the 4th rank
                // we can just simply ask if there was an en'passant move in the move earlier, and if that move was to our left,
                // then using data from the previous state settings we can determine that this pawn has the right to capture
                // the passing pawn.
                if(_state.enPassantFile == (ChessEnpassantFileType)(file - 1u))
                {
                    retvalBitmask |= bitmaskOfLeftsideAttack;
                }
            }
        }

        //similierly to the right side, and only when and if the pawn's file isn't the H file
        if(file < 7u)
        {
            uint64_t bitmaskOfRightsideAttack = bitmaskOfPawn << 9u;
            if((bitmaskOfRightsideAttack & enemyPieces) != 0ull)
            {
                retvalBitmask |= bitmaskOfRightsideAttack;
            }
            else if (rank == 4u)
            {
                // same en passant conditions as above, but now for the other side.
                if(_state.enPassantFile == (ChessEnpassantFileType)(file + 1u))
                {
                    retvalBitmask |= bitmaskOfRightsideAttack;
                }
            }
        }

        // and done with light color
        
    }
    else // color dark
    {
        if(rank <= 0u) return 0ull; // it is on the first rank, so it doesnt have any more moves, can only promote.

        uint64_t forwardMoveMask = bitmaskOfPawn >> 8u; // same as above, but we shift the other way around.
        if((forwardMoveMask & (friendlyPieces | enemyPieces)) == 0ull)
        {
            retvalBitmask |= forwardMoveMask;

            if(rank == 6ull)
            {   
                uint64_t doubleForwardMoveMask = bitmaskOfPawn >> 16u;
                if((doubleForwardMoveMask & (friendlyPieces | enemyPieces)) == 0ull)
                {
                    retvalBitmask |= doubleForwardMoveMask;
                }
            }
        }

        //sideways attack for dark
        if(file < 7u) // if we are anywhere but the A file we can try checking our leftways sideattack.
        {
            uint64_t bitmaskOfLeftsideAttack = bitmaskOfPawn >> 7u;
            if((bitmaskOfLeftsideAttack & enemyPieces) != 0ull)
            {
                retvalBitmask |= bitmaskOfLeftsideAttack;
            }
            else if (rank == 3u)
            {
                if(_state.enPassantFile == (ChessEnpassantFileType)(file + 1u))
                {
                    retvalBitmask |= bitmaskOfLeftsideAttack;
                }
            }
        }

        if(file > 0u)
        {
            uint64_t bitmaskOfRightsideAttack = bitmaskOfPawn >> 9u;
            if((bitmaskOfRightsideAttack & enemyPieces) != 0ull)
            {
                retvalBitmask |= bitmaskOfRightsideAttack;
            }
            else if (rank == 3u)
            {
                if(_state.enPassantFile == (ChessEnpassantFileType)(file - 1u))
                {
                    retvalBitmask |= bitmaskOfRightsideAttack;
                }
            }
        }
    }

    return retvalBitmask;
}

uint64_t ChessGame::GetPseudoLegalMovesOfBishop(uint8_t index, ChessColorType color)
{
    // we know there is a bishop in index, with chesscolor color. (preexisting information)
    //diagonal bitmask offsets:  7  9
    //                          -9 -7
    uint64_t retvalBitmask = 0ull;
    uint64_t bitmaskOfBishop = CreateBitmaskFromIndex(index);

    uint64_t enemyPieces = GetPiecePositionsBitmaskOf(GetInverseColor(color));
    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);

    int startIndex = (int)index;
    int startFile = startIndex % 8;
    int startRank = startIndex / 8;
    for(int i=0;i<4;++i)
    {
        int indexDelta = i % 2 == 0 ? 9 : 7;

        int deltaX = i/2 == 0? 1:-1;
        int deltaY = i%3 == 0? 1: -1;

        int currX = startFile + deltaX;
        int currY = startRank + deltaY;
        uint64_t iteratorBitmask = i % 3 == 0 ? bitmaskOfBishop << indexDelta : bitmaskOfBishop >> indexDelta;
        bool hasReachedEnemyPiece = false;
        while(currX >= 0 && currY >= 0 && currX < 8 && currY < 8 && ((iteratorBitmask & friendlyPieces) == 0ull) && !hasReachedEnemyPiece)
        {
            retvalBitmask |= iteratorBitmask;
            if((iteratorBitmask & enemyPieces) != 0ull)
            {
                hasReachedEnemyPiece = true;
            }

            currX += deltaX;
            currY += deltaY;
            iteratorBitmask = i % 3 == 0 ? iteratorBitmask << indexDelta : iteratorBitmask >> indexDelta;
        }

    }

    return retvalBitmask;
}

uint64_t ChessGame::GetPseudoLegalMovesOfHorse(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;

    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);

    int startFile = index % 8;
    int startRank = index / 8;

    int deltaX = 1;
    int deltaY = 2;
    for(int i=0;i<8;++i)
    {
        int currX = startFile + deltaX;
        int currY = startRank + deltaY;

        if(currX >= 0 && currX<8 && currY >= 0 && currY <8)
        {
            uint64_t moveBitmask = CreateBitmaskFromIndex(currY * 8u + currX);
            if((moveBitmask & friendlyPieces) == 0ull) // no friendly piece is standing there
            {
                retvalBitmask |= moveBitmask;
            }
        }

        if(i%2 == 0)
        {
            int tmp = deltaY;
            deltaY = deltaX;
            deltaX = tmp;
        }
        else
        {
            int tmp = deltaY;
            deltaY = deltaX;
            deltaX = tmp;

            tmp = deltaY;
            deltaY = -deltaX;
            deltaX = tmp;
        }
    }

    return retvalBitmask;
}

uint64_t ChessGame::GetPseudoLegalMovesOfRook(uint8_t index, ChessColorType color)
{
    // we know there is a rook in index, with chesscolor color. (preexisting information)
    //diagonal bitmask offsets:    8
    //                          -1   1 
    //                            -8
    uint64_t retvalBitmask = 0ull;
    uint64_t bitmaskOfRook = CreateBitmaskFromIndex(index);

    uint64_t enemyPieces = GetPiecePositionsBitmaskOf(GetInverseColor(color));
    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);

    int startIndex = (int)index;
    int startFile = startIndex % 8;
    int startRank = startIndex / 8;

    int deltaX = 1;
    int deltaY = 0;
    for(int i=0;i<4;++i)
    {
        int indexDelta = i % 2 == 0 ? 1 : 8;

        int currX = startFile + deltaX;
        int currY = startRank + deltaY;
        uint64_t iteratorBitmask = i % 3 == 0 ? bitmaskOfRook << indexDelta : bitmaskOfRook >> indexDelta;
        bool hasReachedEnemyPiece = false;
        while(currX >= 0 && currY >= 0 && currX < 8 && currY < 8 && ((iteratorBitmask & friendlyPieces) == 0ull) && !hasReachedEnemyPiece)
        {
            retvalBitmask |= iteratorBitmask;
            if((iteratorBitmask & enemyPieces) != 0ull)
            {
                hasReachedEnemyPiece = true;
            }

            currX += deltaX;
            currY += deltaY;
            iteratorBitmask = i % 3 == 0 ? iteratorBitmask << indexDelta : iteratorBitmask >> indexDelta;
        }

        int tmp = deltaY;
        deltaY = -deltaX;
        deltaX = tmp;

    }

    return retvalBitmask;
}

uint64_t ChessGame::GetPseudoLegalMovesOfQueen(uint8_t index, ChessColorType color)
{
    return GetPseudoLegalMovesOfBishop(index,color) | GetPseudoLegalMovesOfRook(index,color);
}

uint64_t ChessGame::GetPseudoLegalMovesOfKing(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;

    uint64_t bitmaskOfKing = CreateBitmaskFromIndex(index);

    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);

    int startFile = index % 8;
    int startRank = index / 8;

    const int offsets[8][2] = 
    {
        {1,1},
        {1,0},
        {1,-1},
        {0,-1},
        {-1,-1},
        {-1,0},
        {-1,1},
        {0,1},
    };

    for(int i=0;i<8;++i)
    {
        int currX = startFile + offsets[i][0];
        int currY = startRank + offsets[i][1];

        if(currX >= 0 && currX < 8 && currY >= 0 && currY < 8)
        {
            uint64_t bitmaskOfMove = CreateBitmaskFromIndex(currY * 8 + currX);
            if((bitmaskOfMove & friendlyPieces) == 0ull)
            {
                retvalBitmask |= bitmaskOfMove;
            }
        }
    }

    ChessColorType enemyColor = GetInverseColor(color);
    uint64_t enemyPieces = GetPiecePositionsBitmaskOf(enemyColor);
    uint64_t enemyAttackMask = GetAttackBitmaskOf(enemyColor);

    if((enemyAttackMask & bitmaskOfKing) != 0ull) return retvalBitmask; // if the king is in check it can't castle

    uint64_t allPieces = enemyPieces | friendlyPieces;

    //lets check castling
    ChessCastlingRightBitmasks kingSideCastlingRightMask = (ChessCastlingRightBitmasks)(1u << ((uint8_t)color * 2u));
    if((kingSideCastlingRightMask & _state.castlingRights) != 0u) // if we can castle king side
    {
        uint64_t firstCellBitmaskToRook = bitmaskOfKing << 1u;
        uint64_t secondCellBitmaskToRook = bitmaskOfKing << 2u;

        if(((firstCellBitmaskToRook | secondCellBitmaskToRook) & allPieces) == 0ull && // if there is no piece between the king and rook
            (firstCellBitmaskToRook & enemyAttackMask) == 0ull) // this is needed, bc it checks if the king attacked while castling
        {
            retvalBitmask |= secondCellBitmaskToRook;
        }
    }

    ChessCastlingRightBitmasks queenSideCastlingRightMask = (ChessCastlingRightBitmasks)(1u << ((uint8_t)color * 2u + 1u));
    if((queenSideCastlingRightMask & _state.castlingRights) != 0u) // if we can castle queenside
    {
        uint64_t firstCellBitmaskToRook = bitmaskOfKing >> 1u;
        uint64_t secondCellBitmaskToRook = bitmaskOfKing >> 2u;
        uint64_t thirdCellBitmaskToRook = bitmaskOfKing >> 3u;

        if(((firstCellBitmaskToRook | secondCellBitmaskToRook | thirdCellBitmaskToRook) & allPieces) == 0ull && 
            (firstCellBitmaskToRook & enemyAttackMask) == 0ull) 
        {
            retvalBitmask |= secondCellBitmaskToRook; // we dont have to check secondcell for enemy attack, because this is only a pseudo
                                                      // legal positional querry. after any pesudolegal move it will be checked that whether
                                                      // or not in the final position the king is in check. (if it is -> not legal move)
        }
    }

    return retvalBitmask;
}

uint64_t ChessGame::GetAttackBitmaskOf(ChessColorType color)
{
    uint64_t retvalAttackMask = 0ull;

    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);
    while(friendlyPieces)
    {
        uint8_t currentFriendlyPieceIndex = GetLeastSignificantBitIndexFromBitmask(friendlyPieces);

        ChessTileData currentFriendlyPieceData = GetTileDataAt(currentFriendlyPieceIndex);

        retvalAttackMask |= GetAttackBitmaskOfPiece(currentFriendlyPieceIndex,currentFriendlyPieceData.piece,currentFriendlyPieceData.color);

        friendlyPieces = RemoveLeastSignificantBitOfBitmask(friendlyPieces);
    }

    return retvalAttackMask;
}

uint64_t ChessGame::GetAttackBitmaskOfPiece(uint8_t index, ChessPieceType piece, ChessColorType color)
{
    switch(piece)
    {
        case PIECE_PAWN:
            return GetAttackBitmaskOfPawn(index,color);
        break;
        case PIECE_BISHOP:
            return GetAttackBitmaskOfBishop(index,color);
        break;
        case PIECE_HORSE:
            return GetAttackBitmaskOfHorse(index,color);
        break;
        case PIECE_ROOK:
            return GetAttackBitmaskOfRook(index,color);
        break;
        case PIECE_QUEEN:
            return GetAttackBitmaskOfQueen(index,color);
        break;
        case PIECE_KING:
            return GetAttackBitmaskOfKing(index,color);
        break;
        default:
            return 0ull;
        break;
    }
}

uint64_t ChessGame::GetAttackBitmaskOfPawn(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;

    uint64_t bitmaskOfPawn = CreateBitmaskFromIndex(index);

    uint8_t rank = index / 8u;
    uint8_t file = index % 8u;
    if(color == COLOR_LIGHT)
    {
        if(rank >= 7u) return 0ull; 

        if(file > 0u) 
        {
            retvalBitmask |= bitmaskOfPawn << 7u;
        }

        if(file < 7u)
        {
            retvalBitmask |= bitmaskOfPawn << 9u;
        }
    }
    else
    {
        if(rank <= 0u) return 0ull; 

       
        if(file < 7u)
        {
            retvalBitmask |= bitmaskOfPawn >> 7u;
        }

        if(file > 0u)
        {
            retvalBitmask |= bitmaskOfPawn >> 9u;
            
        }
    }

    return retvalBitmask;
}

uint64_t ChessGame::GetAttackBitmaskOfBishop(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;
    uint64_t bitmaskOfBishop = CreateBitmaskFromIndex(index);

    uint64_t enemyPieces = GetPiecePositionsBitmaskOf(GetInverseColor(color));
    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);

    uint64_t allPieces = enemyPieces | friendlyPieces;

    const int offsets[4][2] =
    {
        {1,1},
        {1,-1},
        {-1,-1},
        {-1,1},
    };
    
    int startFile = index % 8;
    int startRank = index / 8;
    for(int i=0;i<4;++i)
    {
        int indexDelta = i % 2 == 0 ? 9 : 7;

        int currX = startFile + offsets[i][0];
        int currY = startRank + offsets[i][1];
        uint64_t iteratorBitmask = i % 3 == 0 ? bitmaskOfBishop << indexDelta : bitmaskOfBishop >> indexDelta;
        bool hasReachedAPiece = false;
        while(currX >= 0 && currY >= 0 && currX < 8 && currY < 8 &&  !hasReachedAPiece)
        {
            retvalBitmask |= iteratorBitmask;
            if((iteratorBitmask & allPieces) != 0ull)
            {
                hasReachedAPiece = true;
            }

            currX += offsets[i][0];
            currY += offsets[i][1];
            iteratorBitmask = i % 3 == 0 ? iteratorBitmask << indexDelta : iteratorBitmask >> indexDelta;
        }

    }

    return retvalBitmask;
}

uint64_t ChessGame::GetAttackBitmaskOfHorse(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;

    const int offsets[8][2] = 
    {
        {1,2},
        {2,1},
        {2,-1},
        {1,-2},

        {-1,-2},
        {-2,-1},
        {-2,1},
        {-1,2},
    };

    int startFile = index % 8;
    int startRank = index / 8;
    for(int i=0;i<8;++i)
    {
        int currX = startFile + offsets[i][0];
        int currY = startRank + offsets[i][1];

        if(currX >= 0 && currX<8 && currY >= 0 && currY <8)
        {
            retvalBitmask |= CreateBitmaskFromIndex(currY * 8u + currX);
        }
    }

    return retvalBitmask;
}

uint64_t ChessGame::GetAttackBitmaskOfRook(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;
    uint64_t bitmaskOfRook = CreateBitmaskFromIndex(index);

    uint64_t enemyPieces = GetPiecePositionsBitmaskOf(GetInverseColor(color));
    uint64_t friendlyPieces = GetPiecePositionsBitmaskOf(color);

    uint64_t allPieces = enemyPieces | friendlyPieces;

    const int offsets[4][2] = 
    {
        {1,0},
        {0,-1},
        {-1,0},
        {0,1},
    };

    int startFile = index % 8;
    int startRank = index / 8;
    for(int i=0;i<4;++i)
    {
        int indexDelta = i % 2 == 0 ? 1 : 8;

        int currX = startFile + offsets[i][0];
        int currY = startRank + offsets[i][1];
        uint64_t iteratorBitmask = i % 3 == 0 ? bitmaskOfRook << indexDelta : bitmaskOfRook >> indexDelta;
        bool hasReachedAPiece = false;
        while(currX >= 0 && currY >= 0 && currX < 8 && currY < 8 && !hasReachedAPiece)
        {
            retvalBitmask |= iteratorBitmask;
            if((iteratorBitmask & allPieces) != 0ull)
            {
                hasReachedAPiece = true;
            }

            currX += offsets[i][0];
            currY += offsets[i][1];
            iteratorBitmask = i % 3 == 0 ? iteratorBitmask << indexDelta : iteratorBitmask >> indexDelta;
        }
    }

    return retvalBitmask;
}

uint64_t ChessGame::GetAttackBitmaskOfQueen(uint8_t index, ChessColorType color)
{
    return GetAttackBitmaskOfBishop(index,color) | GetAttackBitmaskOfRook(index,color);
}

uint64_t ChessGame::GetAttackBitmaskOfKing(uint8_t index, ChessColorType color)
{
    uint64_t retvalBitmask = 0ull;

    const int offsets[8][2] = 
    {
        {1,1},
        {1,0},
        {1,-1},
        {0,-1},
        {-1,-1},
        {-1,0},
        {-1,1},
        {0,1},
    };

    int startFile = index % 8;
    int startRank = index / 8;
    for(int i=0;i<8;++i)
    {
        int currX = startFile + offsets[i][0];
        int currY = startRank + offsets[i][1];

        if(currX >= 0 && currX < 8 && currY >= 0 && currY < 8)
        {
            retvalBitmask  |= CreateBitmaskFromIndex(currY * 8 + currX);
        }
    }
    
    return retvalBitmask;
}

void ChessGame::DeletePieceAt(uint8_t index, ChessPieceType piece, ChessColorType color)
{
    uint64_t bitmaskOfIndex = CreateBitmaskFromIndex(index);

    _state.piecePositions[color][piece] &= ~bitmaskOfIndex;
}

void ChessGame::PutPieceAt(uint8_t index, ChessPieceType piece, ChessColorType color)
{
    uint64_t bitmaskOfIndex = CreateBitmaskFromIndex(index);

    _state.piecePositions[color][piece] |= bitmaskOfIndex;
}

void ChessGame::ApplyMove(uint8_t from, uint8_t to)
{
    //prerequisities: this function is only called after getting the pseudo-legal moves, therefore any move that reaches this function
    // can phisically and logically be made, however it might not be "ethically" legal since we have to check if after the move the palyer's
    // king is left attacked or not. 
    ChessTileData fromTileData = GetTileDataAt(from);
    ChessTileData toTileData = GetTileDataAt(to);

    ChessColorType movingPieceColor = fromTileData.color;
    ChessColorType enemyPieceColor = GetInverseColor(movingPieceColor);

    uint8_t fromFile = from % 8u;
    uint8_t fromRank = from / 8u;
    uint8_t toFile = to % 8u;
    uint8_t toRank = to / 8u;

    ChessEnpassantFileType beforeMoveEnPassantFile = (ChessEnpassantFileType)_state.enPassantFile;

    //clear en passant file, but only if no en passant file access was made in this move
    _state.enPassantFile = (uint8_t)FILE_NONE;

    // in the upcoming if and else if branches we check special rules.
    // basic rule of thumb: every move in chess is either: 
    //  friendlyColored piece moves to -> noColor tile with no piece(free tile)
    //  friendlyColored piece moves to -> enemy colored piece(captures it)
    // In the sections below we check for special rules and interactions where the move deviates from the 2 cases listed above.
    if(fromTileData.piece == PIECE_PAWN) // watch out for en passant
    {
        if(std::abs((int)toRank - (int)fromRank) >= 2)
        {
            //The pawn wants to make a pseudo-legal double move, we have to set the enpassant right.
            _state.enPassantFile = toFile;
        }

        //if pawn is attacking sideways, and there is no piece where it attacks we know: en passant is about to happen.
        if(std::abs((int)toRank - (int)fromRank) == 1 && std::abs((int)toFile - (int)fromFile) == 1 && toTileData.piece == PIECE_NONE)
        {
            //now we know en'passant is happening, but we dont know where to kill the piece(except we actually do)
            // the piece we capture through en passant is always on the same rank as we are, and we can determine which side by looking at
            // the file the pawn moved to(if pawn moved left, the to-be captured pawn is on the left too)
            int fileWhereToBeCapturedPawnIs = toFile;
            int rankWhereToBeCapturedPawnIs = fromRank;
            int indexOfToBeCapturedPawn = fileWhereToBeCapturedPawnIs + rankWhereToBeCapturedPawnIs * 8;
            uint8_t indexOfCapturingPosition = (uint8_t)indexOfToBeCapturedPawn;

            //we capture the enemy pawn based on en passant.
            DeletePieceAt(indexOfCapturingPosition,PIECE_PAWN,enemyPieceColor);
        }
    }
    else if(fromTileData.piece == PIECE_KING) // watch out for castling
    {
        // here we have to check if the king was allowed during pseudo-move checking to move 2 squares in a given castling direction
        int fileDelta = (int)toFile - (int)fromFile;
        if(std::abs(fileDelta) >= 2)
        {
            //king is castling, therefore we have to move the given rook to be beside the king.
            // we can calculate the rook's position and the position it should move to by the filedelta of the move
            int rookFromFile = -1;
            int rookFromRank = fromRank; // rank is always same as king
            int rookToFile = -1;
            int rookToRank = fromRank; // same as king again
            if(fileDelta > 0) 
            {
                //castling king side
                // the rook is always at (kingFile + 3,kingRank)
                rookFromFile = (int)fromFile + 3;
                rookToFile = (int)toFile -1; // moves to the left side of the king
            }
            else
            {
                //castling queenside
                // the rook is at (kingFile - 4,kingRank)
                rookFromFile = (int)fromFile - 4;
                rookToFile = (int)toFile + 1; // moves to the right side of the king
            }

            uint8_t indexOfWhereRookIs = (uint8_t)(rookFromFile + rookFromRank * 8);
            uint8_t indexWhereRookWillBe = (uint8_t)(rookToFile + rookToRank * 8);

            DeletePieceAt(indexOfWhereRookIs,PIECE_ROOK,movingPieceColor);
            PutPieceAt(indexWhereRookWillBe,PIECE_ROOK,movingPieceColor);

            
        }

        //and lose all rights to castle for this given moving color
        uint8_t kingSideCastlingRight = 1u << (movingPieceColor * 2u);
        uint8_t queenSideCastlingRight = 1u << (movingPieceColor * 2u + 1u);

        _state.castlingRights &= ~kingSideCastlingRight; // we need the bitwise negation.
        _state.castlingRights &= ~queenSideCastlingRight;
    }
    else if(fromTileData.piece == PIECE_ROOK) // watch out to loose castling right after move
    {
        //if the rook we wanna move is in a corner, and we still have the given castlingright for that rook, we take it away
        uint8_t kingSideCastlingRightOfMovingColor = 1u << (movingPieceColor * 2u);
        uint8_t queenSideCastlingRightOfMovingColor = 1u << (movingPieceColor * 2u + 1u);

        uint8_t baseRankOfColor = movingPieceColor == COLOR_LIGHT?0u:7u;

        if(fromFile == 7u && baseRankOfColor == fromRank && ((_state.castlingRights & kingSideCastlingRightOfMovingColor) != 0u) )
        {
            //this rook we wanna move is positioned on the kingside of the mmoving color, and still has castling right(hasnt moved)
            // therefore, and bc the rook moves we take away the castling right on that side
            _state.castlingRights &= ~kingSideCastlingRightOfMovingColor;
        }
        else if(fromFile == 0u && baseRankOfColor == fromRank && ((_state.castlingRights & queenSideCastlingRightOfMovingColor) != 0u))
        {
            _state.castlingRights &= ~queenSideCastlingRightOfMovingColor;
        }
    }
    
    // Just simply make the base move(move the from piece to the to field.)
    
    if(toTileData.piece == PIECE_ROOK) // if we capture an enemy rook, we have to take away the given castling right.
    {
        // we know this is an enemy piece(rook), therefore we have to take away the castling right for that color too

        uint8_t kingSideCastlingRightOfEnemyColor = 1u << (enemyPieceColor * 2u);
        uint8_t queenSideCastlingRightOfEnemyColor = 1u << (enemyPieceColor * 2u + 1u);

        uint8_t baseRankOfEnemyColor = enemyPieceColor == COLOR_LIGHT?0u:7u;

        if(toFile == 7u && toRank == baseRankOfEnemyColor && ((_state.castlingRights & kingSideCastlingRightOfEnemyColor) != 0u) )
        {
            _state.castlingRights &= ~kingSideCastlingRightOfEnemyColor;
        }
        else if(toFile == 0u && toRank == baseRankOfEnemyColor && ((_state.castlingRights & queenSideCastlingRightOfEnemyColor) != 0u))
        {
            _state.castlingRights &= ~queenSideCastlingRightOfEnemyColor;
        }
    }

    //now all special "side" effects have been dealt with we can make the move
    DeletePieceAt(from,fromTileData.piece,fromTileData.color);
    PutPieceAt(to,fromTileData.piece,fromTileData.color);

    if(toTileData.piece != PIECE_NONE)
    {
        //we captured something with the move. 
        //we must delete the captured piece
        DeletePieceAt(to,toTileData.piece,toTileData.color);
    }

    //we should return something [TODO]
    // for example like if a pawn promoted...but this might not be the best place, since we check 
}

ChessGameStateType ChessGame::FinalizeStateAfterMove()
{
    // Important: a player turn swap has been called before, but this func is called at the end of the previous colors move 
    ChessColorType currentColor = (ChessColorType)_state.currentPlayer;
    ChessColorType enemyColor = GetInverseColor(currentColor);

    uint64_t enemyColorAttackBitmask = GetAttackBitmaskOf(enemyColor);
    bool isCurrentKingUnderAttack = (this->_state.piecePositions[currentColor][PIECE_KING] & enemyColorAttackBitmask) != 0ull;
    bool doesCurrentColorHaveAnyLegalMovesAfterAttack = HasAnyLegalMoves(currentColor);
    if(!doesCurrentColorHaveAnyLegalMovesAfterAttack)
    {
        if(isCurrentKingUnderAttack)
        {
            return ChessGameStateType::STATE_GAME_WON;
        }
        else
        {
            return ChessGameStateType::STATE_DRAW_STALEMATE;
        }
    }

    bool isThereSufficientMaterialToCheckMate = IsThereSufficientMaterialToCheckMate();
    if(isThereSufficientMaterialToCheckMate == false)
    {
        return ChessGameStateType::STATE_DRAW_INSUFFICIENT_MATERIAL;
    }

    //After the main ending cases have been checked, we process position repetition calculations,
    // Meaning the game can enter a state where draw can be requested after either:
    //  - the same position has been repeated atleast 3 times
    //  - 100 moves have passed without capturing a piece or pawn movement(100 half moves happend)
    //
    // the game can literally end by :
    //  - the same position coming up atleast for the 5th time
    //  - halfmovecount reached 150 (75 for each side)

    //Supposed information at this point of spacetime:
    // - this function is only called after a fully legal move was made.
    // - the halfmove counter is only incremented/reset outside, not here! but still, we handle its result here.
    // - we always calculate a zobrist hash no matter what happend, bc the zobrist hash array might have been reset in the outside, but we
    //   dont give a hot fuck about that.

    uint64_t currentPositionsZobristHash = CalculateZobristHashOfCurrentState();
    int amountOfTimesThisHashAppearsInStorage = 1; // atleast one(the current)
    for(int i = 0;i<this->_previousZobristHashes.size();++i)
    {
        if(currentPositionsZobristHash == this->_previousZobristHashes[i])
        {
            ++amountOfTimesThisHashAppearsInStorage;
        }
    }

    //lets put the new zobrist hash inside the storage (so we dont check it before)
    this->_previousZobristHashes.push_back(currentPositionsZobristHash);

    if(amountOfTimesThisHashAppearsInStorage >= 5 || this->_halfMoveCount >= 150u)
    {
        return ChessGameStateType::STATE_DRAW_REPETITION;
    }
    else if(amountOfTimesThisHashAppearsInStorage >= 3 || this->_halfMoveCount >= 100u)
    {
        return ChessGameStateType::STATE_DRAW_REQUESTABLE;
    }

    return ChessGameStateType::STATE_PLAYING;
}

bool ChessGame::HasAnyLegalMoves(ChessColorType color)
{
    uint64_t piecesOfColor = GetPiecePositionsBitmaskOf(color);
    bool hasFoundLegalMove = false;
    while(piecesOfColor && !hasFoundLegalMove)
    {
        uint8_t pieceIndex = GetLeastSignificantBitIndexFromBitmask(piecesOfColor);
        ChessTileData tileData = GetTileDataAt(pieceIndex);

        uint64_t pseudoLegalMoves =  GetPseudoLegalMovesOfPiece(pieceIndex,tileData.piece,color);

        while(pseudoLegalMoves && !hasFoundLegalMove)
        {
            uint8_t toIndex = GetLeastSignificantBitIndexFromBitmask(pseudoLegalMoves);

            ChessGameState stateBeforeMove = _state;

            ApplyMove(pieceIndex,toIndex);

            uint64_t attackBitmaskOfEnemy = GetAttackBitmaskOf(GetInverseColor(color));
            if( (_state.piecePositions[color][PIECE_KING] & attackBitmaskOfEnemy) == 0ull)// a pseudolegal move is legal if the king is in safety
            {
                hasFoundLegalMove = true;
            }

            _state = stateBeforeMove;

            pseudoLegalMoves = RemoveLeastSignificantBitOfBitmask(pseudoLegalMoves);
        }

        piecesOfColor = RemoveLeastSignificantBitOfBitmask(piecesOfColor);
    }

    return hasFoundLegalMove;
}

bool ChessGame::IsThereSufficientMaterialToCheckMate()
{
    ChessColorType currentColor = GetWhoseTurnItIs();
    ChessColorType enemyColor = GetOpponentsColor();

    if( this->_state.piecePositions[currentColor][PIECE_QUEEN] != 0ull ||
        this->_state.piecePositions[enemyColor][PIECE_QUEEN] != 0ull ||
        this->_state.piecePositions[currentColor][PIECE_ROOK] != 0ull ||
        this->_state.piecePositions[enemyColor][PIECE_ROOK] != 0ull ||
        this->_state.piecePositions[currentColor][PIECE_PAWN] != 0ull ||
        this->_state.piecePositions[enemyColor][PIECE_PAWN] != 0ull)
    {
        //If we get here that means checkmate can be more than possible
        return true;
    }

    uint64_t currentColorHorses = this->_state.piecePositions[currentColor][PIECE_HORSE];
    uint64_t enemyColorHorses = this->_state.piecePositions[enemyColor][PIECE_HORSE];
    uint64_t currentColorBishops = this->_state.piecePositions[currentColor][PIECE_BISHOP];
    uint64_t enemyColorBishops = this->_state.piecePositions[enemyColor][PIECE_BISHOP];

    uint8_t currentColorHorseCount = CountOneBitsOfBitmask(currentColorHorses);
    uint8_t enemyColorHorseCount = CountOneBitsOfBitmask(enemyColorHorses);
    uint8_t currentColorBishopCount = CountOneBitsOfBitmask(currentColorBishops);
    uint8_t enemyColorBishopCount = CountOneBitsOfBitmask(enemyColorBishops);

    /*
    4 main cases where there just isnt sufficient amount of material:
    king -king
    king + bishop - king
    king + horse - king
    king + bishop - king + bishop (if bishops on same colored tiles)
    */

    uint8_t allHorseCount = currentColorHorseCount + enemyColorHorseCount;
    uint8_t allBishopCount = currentColorBishopCount + enemyColorBishopCount;

    if(allHorseCount == 0u && allBishopCount == 0u) // king - king
    {
        return false;
    }
    else if(allHorseCount == 0u && allBishopCount == 1u) // king + bishop - king
    {
        return false;
    }
    else if(allBishopCount == 0u && allHorseCount == 1u) // king+ horse - king
    {
        return false;
    }
    else if(allHorseCount == 0u && currentColorBishopCount == 1u && enemyColorBishopCount == 1u) // king + bishop - king + bishop
    {
        // if these opposing bishops are on the same tile color, its also insufficient material.
        uint8_t indexOfCurrentColorBishop = GetLeastSignificantBitIndexFromBitmask(currentColorBishops);
        uint8_t rankOfCurrentColorBishop = indexOfCurrentColorBishop / 8u;
        uint8_t fileOfCurrentColorBishop = indexOfCurrentColorBishop % 8u;

        uint8_t indexOfEnemyColorBishop = GetLeastSignificantBitIndexFromBitmask(enemyColorBishops);
        uint8_t rankOfEnemyColorBishop = indexOfEnemyColorBishop / 8u;
        uint8_t fileOfEnemyColorBishop = indexOfEnemyColorBishop % 8u;

        uint8_t currentColorBishopTileCheckvalue = rankOfCurrentColorBishop + fileOfCurrentColorBishop;
        uint8_t enemyColorBishopTileCheckvalue = rankOfEnemyColorBishop + fileOfEnemyColorBishop;
        if(currentColorBishopTileCheckvalue % 2 == enemyColorBishopTileCheckvalue % 2)
        {
            return false;
        }
    }

    //otherwise
    return true;
}

uint8_t ChessGame::GetValidEnPassantFileForCurrentPlayer()
{
    if(this->_state.enPassantFile == FILE_NONE) return FILE_NONE;

    // now we have to check whether or not the one or two pawn places that could attack this en passing pawn actually exist
    // information we know: the other player has made an en passant pawn move, 
    //  meaning they moved a pawn 2 squares in the given en passant file
    ChessColorType currentPlayerColor = GetWhoseTurnItIs();
    ChessColorType enemyPlayerColor = GetOpponentsColor();
    
    uint8_t enPassantRank = currentPlayerColor == COLOR_LIGHT ? 4u : 3u; // the rank where the en passant pawn is;
    uint8_t indexOfEnPassantPawn = enPassantRank * 8u + this->_state.enPassantFile;
    uint64_t bitmaskOfEnPassantPawn = CreateBitmaskFromIndex(indexOfEnPassantPawn);

    uint8_t behindEnPassantRank = currentPlayerColor == COLOR_LIGHT ? 5u : 2u; // the rank where the en passant pawn is
    uint8_t behindEnPassantIndex = behindEnPassantRank * 8u + this->_state.enPassantFile;

    uint8_t indicesOfPawnsBesideEnPassantPawn[2] = {64u,64u};
 
    if(this->_state.enPassantFile > FILE_A && ((bitmaskOfEnPassantPawn >> 1u) & this->_state.piecePositions[currentPlayerColor][PIECE_PAWN]) )
    {
        //minus neighbour exists
        indicesOfPawnsBesideEnPassantPawn[0] = indexOfEnPassantPawn - 1u;
    }

    if(this->_state.enPassantFile < FILE_H && ((bitmaskOfEnPassantPawn << 1u) & this->_state.piecePositions[currentPlayerColor][PIECE_PAWN]) )
    {
        //positive neighbour exists
        indicesOfPawnsBesideEnPassantPawn[1] = indexOfEnPassantPawn + 1u;
    }


    bool foundValidEnPassantMove = false;
    for(int potentialPawn = 0;potentialPawn < 2 && !foundValidEnPassantMove; ++potentialPawn)
    {
        if(indicesOfPawnsBesideEnPassantPawn[potentialPawn] >= 64u) continue;

        //the move is now: this potential pawn, to behind the en passant pawn.
        ChessGameState stateBeforeMove = this->_state;

        ApplyMove(indicesOfPawnsBesideEnPassantPawn[potentialPawn],behindEnPassantIndex);

        uint64_t attackBitmaskOfEnemy = GetAttackBitmaskOf(enemyPlayerColor);
        if( (_state.piecePositions[currentPlayerColor][PIECE_KING] & attackBitmaskOfEnemy) == 0ull)
        {
            foundValidEnPassantMove = true;
        }

        _state = stateBeforeMove;
    }

    return (foundValidEnPassantMove == true ? _state.enPassantFile : FILE_NONE);
}

uint64_t ChessGame::CalculateZobristHashOfCurrentState()
{
    uint64_t retvalZobristHash = 0ull;

    for(int color = 0; color < 2 ;++color)
    {
        for(int piece = 0; piece < 6 ; ++ piece)
        {
            uint64_t currentPiecesBitmask = this->_state.piecePositions[color][piece];
            while(currentPiecesBitmask)
            {
                uint8_t currentPieceIndex = GetLeastSignificantBitIndexFromBitmask(currentPiecesBitmask);

                retvalZobristHash ^= this->_defaultZobristKeys.pieceHashes[color][piece][currentPieceIndex];

                currentPiecesBitmask = RemoveLeastSignificantBitOfBitmask(currentPiecesBitmask);
            }
        }
    }

    //this isnt right, en passant file is only valid for this hashing if after this move the enemy could capture the passing pawn legally
    uint8_t validEnpassantFile = GetValidEnPassantFileForCurrentPlayer();
    if(validEnpassantFile <= FILE_NONE) // there is enpassant right, but it the enemy might not have a move to capture it!!
    {
        retvalZobristHash ^= this->_defaultZobristKeys.enPassantHashes[validEnpassantFile];
    }

    //side to move(whos turn it is)
    retvalZobristHash ^= this->_defaultZobristKeys.sideHashes[this->GetWhoseTurnItIs()];

    //castling right combination's current hash:
    retvalZobristHash ^= this->_defaultZobristKeys.castlingHashes[this->_state.castlingRights]; //castlingRights always 4 bit number, 15u at max

    return retvalZobristHash;
}


uint64_t ChessGame::CreateBitmaskFromIndex(uint8_t index)
{
    if(index >= 64u) return 0ull;

	return 1ull << index;
}

uint8_t ChessGame::GetLeastSignificantBitIndexFromBitmask(uint64_t bitmask)
{
    if (bitmask == 0ull) return 64u;

    uint8_t count = 0u;

    if ((bitmask & 0x00000000FFFFFFFFULL) == 0ull) { count += 32u; bitmask >>= 32u; }
    if ((bitmask & 0x000000000000FFFFULL) == 0ull) { count += 16u; bitmask >>= 16u; }
    if ((bitmask & 0x00000000000000FFULL) == 0ull) { count += 8u;  bitmask >>= 8u;  }
    if ((bitmask & 0x000000000000000FULL) == 0ull) { count += 4u;  bitmask >>= 4u;  }
    if ((bitmask & 0x0000000000000003ULL) == 0ull) { count += 2u;  bitmask >>= 2u;  }
    if ((bitmask & 0x0000000000000001ULL) == 0ull) { count += 1u; }

    return count;
}

uint64_t ChessGame::RemoveLeastSignificantBitOfBitmask(uint64_t bitmask)
{
	if(bitmask == 0ull) return 0ull;

    return bitmask & (bitmask - 1ull);
}

uint8_t ChessGame::CountOneBitsOfBitmask(uint64_t bitmask)
{
    if(bitmask == 0ull) return 0u;

    uint8_t count = 0u;
    while(bitmask)
    {
        bitmask = RemoveLeastSignificantBitOfBitmask(bitmask);

        ++count;
    }

    return count;
}

void ChessGame::SetupDefaultZobristKeys()
{
    std::mt19937_64 randomGenerator(DEFAULT_ZOBRIST_SEED);

    for(int color = 0;color<2;++color)
    {
        for(int piece = 0;piece<6;++piece)
        {
            for(int positionIndex = 0; positionIndex < 64 ;++positionIndex)
            {
                this->_defaultZobristKeys.pieceHashes[color][piece][positionIndex] = randomGenerator();
            }
        }
    }

    for(int castlingCombinationIndex = 0; castlingCombinationIndex < 16 ;++castlingCombinationIndex)
    {
        this->_defaultZobristKeys.castlingHashes[castlingCombinationIndex] = randomGenerator();
    }

    for(int enPassantFile = 0;enPassantFile < 8 ;++enPassantFile)
    {
        this->_defaultZobristKeys.enPassantHashes[enPassantFile] = randomGenerator();
    }

    this->_defaultZobristKeys.sideHashes[0] = randomGenerator();
    this->_defaultZobristKeys.sideHashes[1] = randomGenerator();
    
}
