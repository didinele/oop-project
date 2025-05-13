#pragma once

#include "piece.h"

namespace Game
{
class KnightPiece : public Piece
{
  public:
    KnightPiece(Color color, Coordinates coords);
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;
};
} // namespace Game
