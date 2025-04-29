#include "PawnPiece.h"
#include "../../Util/Debug.h"

// For all intents and purposes, "up" can mean down for black.

namespace game
{
std::vector<Move> PawnPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> out;

    if (enPassantMoves.size())
    {
        for (auto move : enPassantMoves)
        {
            out.push_back(move);
        }
    }

    auto rank_offset = m_Color == Color::White ? 1 : -1;

    auto one_up = m_Coordinates.NewWithRank(m_Coordinates.GetRank() + rank_offset);
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

        auto two_up = one_up.NewWithRank(one_up.GetRank() + rank_offset);
        if (!m_Moved && two_up.IsValid() && !BOARD_AT(two_up).has_value())
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

void PawnPiece::MakeMove(Board &board, Move move, bool simulate)
{
    Piece::MakeMove(board, move, simulate);

    if (simulate)
    {
        return;
    }

    m_Moved = true;

    // Check if we moved 2
    auto diff = move.to.GetRank() - move.from.GetRank();
    if (diff == 2 || diff == -2)
    {
        // Inversion. If this is a white pawn that moved 2, it'd be a black pawn doing en passant,
        // and vice versa
        auto offset = m_Color == Color::White ? -1 : 1;

        // Check if enemy pawns can en passant ours
        auto left = move.to.NewWithFile(move.to.GetFile() - 1);
        if (left.IsValid() && BOARD_AT(left).has_value())
        {
            auto pawn = dynamic_cast<PawnPiece *>(BOARD_AT(left).value());
            if (pawn && pawn->GetColor() != m_Color)
            {
                auto to = Coordinates(m_Coordinates.GetRank() + offset, m_Coordinates.GetFile());
                util::Debugger::Debug(
                    "[PawnPiece::MakeMove] en passant left to %d,%d\n",
                    to.GetRank(),
                    to.GetFile()
                );
                pawn->enPassantMoves.push_back(Move(pawn->m_Coordinates, to, m_Coordinates));
            }
        }

        auto right = move.to.NewWithFile(move.to.GetFile() + 1);
        if (right.IsValid() && BOARD_AT(right).has_value())
        {
            auto pawn = dynamic_cast<PawnPiece *>(BOARD_AT(right).value());
            if (pawn && pawn->GetColor() != m_Color)
            {
                auto to = Coordinates(m_Coordinates.GetRank() + offset, m_Coordinates.GetFile());
                util::Debugger::Debug(
                    "[PawnPiece::MakeMove] en passant right to %d,%d\n",
                    to.GetRank(),
                    to.GetFile()
                );
                pawn->enPassantMoves.push_back(Move(pawn->m_Coordinates, to, m_Coordinates));
            }
        }
    }
}

Piece *PawnPiece::Clone() const
{
    return new PawnPiece(*this);
}
} // namespace game
