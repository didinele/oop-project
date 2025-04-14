#include "KingPiece.h"
#include <vector>

namespace game
{
std::vector<Move> KingPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> out;

    if (canCastleLong)
    {
        auto to = m_Color == Color::White ? Coordinates(0, 2) : Coordinates(7, 2);
        out.push_back(Move(m_Coordinates, to, CastleKind::Long));
    }

    if (canCastleShort)
    {
        auto to = m_Color == Color::White ? Coordinates(0, 6) : Coordinates(7, 6);
        out.push_back(Move(m_Coordinates, to, CastleKind::Short));
    }

    short offsets[8][2] = {{1, 0}, {1, 1}, {1, -1}, {-1, 0}, {-1, 1}, {-1, -1}, {0, 1}, {0, -1}};

    for (auto &offset : offsets)
    {
        auto rankOffset = offset[0];
        auto fileOffset = offset[1];

        auto to =
            Coordinates(m_Coordinates.GetRank() + rankOffset, m_Coordinates.GetFile() - fileOffset);
        if (!to.IsValid())
        {
            continue;
        }

        auto piece = BOARD_AT(to);
        if (piece.has_value())
        {
            if (piece.value()->GetColor() != m_Color && piece.value()->GetSeenBy(board).empty())
            {
                out.push_back(Move(m_Coordinates, to));
            }
        }
        else
        {
            out.push_back(Move(m_Coordinates, to));
        }
    }

    return out;
}
} // namespace game
