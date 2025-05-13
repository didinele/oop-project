#pragma once

#include "piece.h"

namespace Game
{
class KingPiece : public Piece
{
  public:
    KingPiece(Color color, Coordinates coords);
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;

    [[nodiscard]] bool GetIsMated() const;
    void SetMated();

    [[nodiscard]] bool IsInCheck(Board &board) const;

  private:
    bool m_moved = false;
    bool m_mated = false;

    [[nodiscard]] bool _CanCastleShort(Board &board) const;
    [[nodiscard]] bool _CanCastleLong(Board &board) const;

    void _MakeMove(Board &board, Move move, bool simulate) override;
};
} // namespace Game
