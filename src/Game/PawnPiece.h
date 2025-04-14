#pragma once

#include "Piece.h"
#include <optional>

namespace game
{
class PawnPiece : public Piece
{
  public:
    using Piece::Piece;
    std::vector<Move> GetPossibleMoves(std::array<std::array<std::optional<Piece *>, 8>, 8> &board
    ) const override;
    std::optional<Move> enPassantMove;
};
} // namespace game
