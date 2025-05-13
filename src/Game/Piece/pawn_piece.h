#pragma once

#include "piece.h"

namespace Game
{
class PawnPiece : public Piece
{
  public:
    PawnPiece(Color color, Coordinates coords);
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;

    void SetEnPassantMoves(std::vector<Move> moves);

  private:
    bool m_moved = false;
    std::vector<Move> m_en_passant_moves;

    void _MakeMove(Board &board, Move move, bool simulate) override;
};
} // namespace Game
