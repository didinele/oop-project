#pragma once

#include "Piece.h"
#include <optional>

namespace game
{
class PawnPiece : public Piece
{
  public:
    PawnPiece(Color color, Coordinates coords) : Piece(color, coords)
    {
    }
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;
    std::optional<Move> enPassantMove;
};
} // namespace game
