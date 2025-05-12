#include "RookPiece.h"

namespace game
{
RookPiece::RookPiece(Color color, Coordinates coords)
    : Piece(color, coords), m_Kind(RookKind::Noop), m_Moved(true)
{
}
RookPiece::RookPiece(Color color, Coordinates coords, RookKind kind)
    : Piece(color, coords), m_Kind(kind), m_Moved(false)
{
}

std::vector<Move> RookPiece::GetPossibleMoves(Board &board) const
{
    return GetNaiveMovesInDirections(
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
    auto other = new RookPiece(m_Color, m_Coordinates, m_Kind);
    other->m_Moved = m_Moved;

    return other;
}

RookKind RookPiece::GetKind() const
{
    return m_Kind;
}

bool RookPiece::GetHasMoved() const
{
    return m_Moved;
}

void RookPiece::SetMoved()
{
    m_Moved = true;
}

void RookPiece::MakeMove(Board &board, Move move, bool simulate)
{
    Piece::MakeMove(board, move, simulate);
    if (!simulate)
    {
        m_Moved = true;
    }
}
} // namespace game
