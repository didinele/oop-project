#include "PawnPiece.h"

namespace game
{
std::vector<Move> PawnPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> out;

    if (enPassantMove)
    {
        out.push_back(*enPassantMove);
    }

    auto one_up = m_Coordinates.NewWithRank(m_Coordinates.GetRank() + 1);
    if (one_up.IsValid() && !BOARD_AT(one_up).has_value())
    {
        if (one_up.IsPromotionSquare(m_Color))
        {
            auto moves = Move::GetPromotionMoves(m_Coordinates, one_up);
            for (auto move : moves)
                out.push_back(move);
        }
        else
        {
            out.push_back(Move(m_Coordinates, one_up));
        }

        auto two_up = one_up.NewWithRank(one_up.GetRank() + 1);
        if (two_up.IsValid() && !BOARD_AT(two_up).has_value())
        {
            if (two_up.IsPromotionSquare(m_Color))
            {
                auto moves = Move::GetPromotionMoves(m_Coordinates, two_up);
                for (auto move : moves)
                    out.push_back(move);
            }
            else
            {
                out.push_back(Move(m_Coordinates, two_up));
            }
        }
    }

    auto left = one_up.NewWithFile(one_up.GetFile() - 1);
    if (left.IsValid() && BOARD_AT(left).has_value() &&
        BOARD_AT(left).value()->GetColor() != m_Color)
    {
        if (left.IsPromotionSquare(m_Color))
        {
            auto moves = Move::GetPromotionMoves(m_Coordinates, left);
            for (auto move : moves)
                out.push_back(move);
        }
        else
        {
            out.push_back(Move(m_Coordinates, left));
        }
    }

    auto right = one_up.NewWithFile(one_up.GetFile() + 1);
    if (right.IsValid() && BOARD_AT(right).has_value() &&
        BOARD_AT(right).value()->GetColor() != m_Color)
    {
        if (right.IsPromotionSquare(m_Color))
        {
            auto moves = Move::GetPromotionMoves(m_Coordinates, right);
            for (auto move : moves)
                out.push_back(move);
        }
        else
        {
            out.push_back(Move(m_Coordinates, right));
        }
    }

    return out;
}
Piece *PawnPiece::Clone() const
{
    return new PawnPiece(m_Color, m_Coordinates);
}
} // namespace game
