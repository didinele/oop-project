#pragma once

#include "Piece.h"

namespace game
{
class KingPiece : public Piece
{
  public:
    KingPiece(Color color, Coordinates coords) : Piece(color, coords)
    {
    }
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;

    bool mated = false;

    [[nodiscard]] bool IsInCheck(Board &board) const;

  private:
    bool m_HasMoved = false;

    [[nodiscard]] bool CanCastleShort(Board &board) const;
    [[nodiscard]] bool CanCastleLong(Board &board) const;

    void MakeMove(Board &board, Move move, bool simulate) override;
};
} // namespace game
