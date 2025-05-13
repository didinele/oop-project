#pragma once

#include "piece.h"

namespace Game
{
enum class RookKind
{
    // Used when promoting to a rook
    Noop,
    Short,
    Long
};

class RookPiece : public Piece
{
  public:
    RookPiece(Color color, Coordinates coords);
    RookPiece(Color color, Coordinates coords, RookKind kind);
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;
    [[nodiscard]] virtual Piece *Clone() const override;

    [[nodiscard]] RookKind GetKind() const;

    [[nodiscard]] bool GetHasMoved() const;
    void SetHasMoved();

  private:
    RookKind m_kind;
    bool m_moved;

    void _MakeMove(Board &board, Move move, bool simulate) override;
};
} // namespace Game
