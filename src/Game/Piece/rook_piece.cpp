#include "rook_piece.h"

namespace Game
{
RookPiece::RookPiece(Color color, Coordinates coords)
    : Piece(color, coords), m_kind(RookKind::Noop), m_moved(true)
{
}
RookPiece::RookPiece(Color color, Coordinates coords, RookKind kind)
    : Piece(color, coords), m_kind(kind), m_moved(false)
{
}

std::vector<Move> RookPiece::GetPossibleMoves(Board &board) const
{
    return _GetNaiveMovesInDirections(
        board,
        std::vector<std::array<short, 2>>{
            {1, 0},  // up
            {-1, 0}, // down
            {0, -1}, // left
            {0, 1},  // right
        }
    );
}

Piece *RookPiece::Clone() const
{
    auto other = new RookPiece(m_color, m_coordinates, m_kind);
    other->m_moved = m_moved;

    return other;
}

RookKind RookPiece::GetKind() const
{
    return m_kind;
}

bool RookPiece::GetHasMoved() const
{
    return m_moved;
}

void RookPiece::SetHasMoved()
{
    m_moved = true;
}

void RookPiece::_MakeMove(Board &board, Move move, bool simulate)
{
    Piece::_MakeMove(board, move, simulate);
    if (!simulate)
    {
        m_moved = true;
    }
}
} // namespace Game
