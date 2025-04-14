#pragma once

#include "Piece.h"

namespace game
{
class KingPiece : public Piece
{
  public:
    using Piece::Piece;
    std::vector<Move> GetPossibleMoves(Board &board) const override;
    bool isInCheck = false;
    bool canCastleShort = false;
    bool canCastleLong = false;
};
} // namespace game
