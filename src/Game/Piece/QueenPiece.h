#pragma once

#include "Piece.h"

namespace game
{
class QueenPiece : public Piece
{
  public:
    using Piece::Piece;
    std::vector<Move> GetPossibleMoves(Board &board) const override;
};
} // namespace game

