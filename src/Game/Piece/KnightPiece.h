#pragma once

#include "Piece.h"

namespace game
{
class KnightPiece : public Piece
{
  public:
    KnightPiece(Color color, Coordinates coords) : Piece(color, coords)
    {
    }
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
};
} // namespace game
