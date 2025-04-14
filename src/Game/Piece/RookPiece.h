#pragma once

#include "Piece.h"

namespace game
{
class RookPiece : public Piece
{
  public:
    using Piece::Piece;
    std::vector<Move> GetPossibleMoves(Board &board) const override;
};
} // namespace game
