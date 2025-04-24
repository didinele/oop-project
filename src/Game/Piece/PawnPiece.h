#pragma once

#include "Piece.h"

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
    std::vector<Move> enPassantMoves;

    void MakeMove(Board &board, Move move, bool simulate) override;

  private:
    bool m_Moved = false;
};
} // namespace game
