#include "KnightPiece.h"
#include <vector>

namespace game
{
std::vector<Move> KnightPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> moves;

    short offsets[8][2] = {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2}};
    for (auto &offset : offsets)
    {
        auto rank_offset = offset[0];
        auto file_offset = offset[1];

        auto to = Coordinates(
            m_Coordinates.GetRank() + rank_offset,
            m_Coordinates.GetFile() + file_offset
        );

        if (!to.IsValid())
        {
            continue;
        }

        auto piece = BOARD_AT(to);
        if (piece.has_value())
        {
            if (piece.value()->GetColor() != m_Color)
            {
                moves.push_back(Move(m_Coordinates, to));
            }
        }
        else
        {
            moves.push_back(Move(m_Coordinates, to));
        }
    }

    return moves;
}
Piece *KnightPiece::Clone() const
{
    return new KnightPiece(m_Color, m_Coordinates);
}
} // namespace game
