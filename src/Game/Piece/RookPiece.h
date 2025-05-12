#pragma once

#include "Piece.h"

namespace game
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
    void SetMoved();

  private:
    RookKind m_Kind;
    bool m_Moved;

    void MakeMove(Board &board, Move move, bool simulate) override;
};
} // namespace game
