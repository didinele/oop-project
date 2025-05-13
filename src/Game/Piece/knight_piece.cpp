#include "knight_piece.h"
#include <vector>

namespace Game
{
KnightPiece::KnightPiece(Color color, Coordinates coords) : Piece(color, coords)
{
}

std::vector<Move> KnightPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> moves;

    short offsets[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
    for (auto &offset : offsets)
    {
        auto rank_offset = offset[0];
        auto file_offset = offset[1];

        auto to = Coordinates(
            m_coordinates.GetRank() + rank_offset,
            m_coordinates.GetFile() + file_offset
        );

        if (!to.IsValid())
        {
            continue;
        }

        auto piece = BOARD_AT(to);
        if (piece.has_value())
        {
            if (piece.value()->GetColor() != m_color)
            {
                moves.push_back(Move(m_coordinates, to));
            }
        }
        else
        {
            moves.push_back(Move(m_coordinates, to));
        }
    }

    return moves;
}

Piece *KnightPiece::Clone() const
{
    return new KnightPiece(m_color, m_coordinates);
}
} // namespace Game
