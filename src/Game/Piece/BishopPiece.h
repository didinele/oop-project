#pragma once

#include "Piece.h"

namespace game
{
class BishopPiece : public Piece
{
  public:
    BishopPiece(Color color, Coordinates coords) : Piece(color, coords)
    {
    }
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;
};
} // namespace game
