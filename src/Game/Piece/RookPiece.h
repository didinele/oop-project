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
    RookPiece(Color color, Coordinates coords)
        : Piece(color, coords), moved(true), m_Kind(RookKind::Noop)
    {
    }
    RookPiece(Color color, Coordinates coords, RookKind kind)
        : Piece(color, coords), moved(false), m_Kind(kind)
    {
    }
    [[nodiscard]] std::vector<Move> GetPossibleMoves(Board &board) const override;

    bool moved;

    [[nodiscard]] RookKind GetKind() const;

  private:
    RookKind m_Kind;

    void MakeMove(Board &board, Move move, bool simulate) override;
};
} // namespace game
