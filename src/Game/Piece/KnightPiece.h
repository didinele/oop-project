#pragma once

#include "Piece.h"

namespace game
{
class KnightPiece : public Piece
{
  public:
    using Piece::Piece;
    std::vector<Move> GetPossibleMoves(Board &board) const override;
};
} // namespace game

