#pragma once

#include "Piece.h"

namespace game
{
class PawnPiece : public Piece
{
  public:
    PawnPiece(Color color, Coordinates coords);
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;

    void MakeMove(Board &board, Move move, bool simulate) override;

    void SetEnPassantMoves(std::vector<Move> moves);

  private:
    bool m_Moved = false;
    std::vector<Move> m_EnPassantMoves;
};
} // namespace game
