#pragma once

#include "piece.h"

namespace game
{
class BishopPiece : public Piece
{
  public:
    BishopPiece(Color color, Coordinates coords);
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] Piece *Clone() const override;
};
} // namespace game
