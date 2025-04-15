#pragma once

#include "Piece.h"

namespace game
{
class QueenPiece : public Piece
{
  public:
    QueenPiece(Color color, Coordinates coords) : Piece(color, coords)
    {
    }
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
};
} // namespace game
