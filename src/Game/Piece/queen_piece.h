#pragma once

#include "piece.h"

namespace game
{
class QueenPiece : public Piece
{
  public:
    QueenPiece(Color color, Coordinates coords);
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;
};
} // namespace game
