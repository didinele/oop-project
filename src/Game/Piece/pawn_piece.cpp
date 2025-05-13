#include "pawn_piece.h"
#include "../../Util/debug.h"
#include <vector>

// For all intents and purposes, "up" can mean down for black.

namespace Game
{
PawnPiece::PawnPiece(Color color, Coordinates coords) : Piece(color, coords)
{
}

std::vector<Move> PawnPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> out;

    if (m_en_passant_moves.size())
    {
        for (auto move : m_en_passant_moves)
        {
            out.push_back(move);
        }
    }

    auto rank_offset = m_color == Color::White ? 1 : -1;

    auto one_up = m_coordinates.NewWithRank(m_coordinates.GetRank() + rank_offset);
    if (one_up.IsValid() && !BOARD_AT(one_up).has_value())
    {
        if (one_up.IsPromotionSquare(m_color))
        {
            auto moves = Move::GetPromotionMoves(m_coordinates, one_up);
            for (auto move : moves)
                out.push_back(move);
        }
        else
        {
            out.push_back(Move(m_coordinates, one_up));
        }

        auto two_up = one_up.NewWithRank(one_up.GetRank() + rank_offset);
        if (!m_moved && two_up.IsValid() && !BOARD_AT(two_up).has_value())
        {
            if (two_up.IsPromotionSquare(m_color))
            {
                auto moves = Move::GetPromotionMoves(m_coordinates, two_up);
                for (auto move : moves)
                    out.push_back(move);
            }
            else
            {
                out.push_back(Move(m_coordinates, two_up));
            }
        }
    }

    auto left = one_up.NewWithFile(one_up.GetFile() - 1);
    if (left.IsValid() && BOARD_AT(left).has_value() &&
        BOARD_AT(left).value()->GetColor() != m_color)
    {
        if (left.IsPromotionSquare(m_color))
        {
            auto moves = Move::GetPromotionMoves(m_coordinates, left);
            for (auto move : moves)
                out.push_back(move);
        }
        else
        {
            out.push_back(Move(m_coordinates, left));
        }
    }

    auto right = one_up.NewWithFile(one_up.GetFile() + 1);
    if (right.IsValid() && BOARD_AT(right).has_value() &&
        BOARD_AT(right).value()->GetColor() != m_color)
    {
        if (right.IsPromotionSquare(m_color))
        {
            auto moves = Move::GetPromotionMoves(m_coordinates, right);
            for (auto move : moves)
                out.push_back(move);
        }
        else
        {
            out.push_back(Move(m_coordinates, right));
        }
    }

    return out;
}

Piece *PawnPiece::Clone() const
{
    auto other = new PawnPiece(m_color, m_coordinates);
    other->m_moved = m_moved;
    other->m_en_passant_moves = m_en_passant_moves;

    return other;
}

void PawnPiece::_MakeMove(Board &board, Move move, bool simulate)
{
    Piece::_MakeMove(board, move, simulate);

    if (simulate)
    {
        return;
    }

    m_moved = true;

    // Check if we moved 2
    auto diff = move.to.GetRank() - move.from.GetRank();
    if (diff == 2 || diff == -2)
    {
        // Inversion. If this is a white pawn that moved 2, it'd be a black pawn doing en passant,
        // and vice versa
        auto offset = m_color == Color::White ? -1 : 1;

        // Check if enemy pawns can en passant ours
        auto left = move.to.NewWithFile(move.to.GetFile() - 1);
        if (left.IsValid() && BOARD_AT(left).has_value())
        {
            auto pawn = dynamic_cast<PawnPiece *>(BOARD_AT(left).value());
            if (pawn && pawn->GetColor() != m_color)
            {
                auto to = Coordinates(m_coordinates.GetRank() + offset, m_coordinates.GetFile());
                Util::Debugger::Debug(
                    "[PawnPiece::MakeMove] en passant left to %d,%d\n",
                    to.GetRank(),
                    to.GetFile()
                );
                pawn->m_en_passant_moves.push_back(Move(pawn->m_coordinates, to, m_coordinates));
            }
        }

        auto right = move.to.NewWithFile(move.to.GetFile() + 1);
        if (right.IsValid() && BOARD_AT(right).has_value())
        {
            auto pawn = dynamic_cast<PawnPiece *>(BOARD_AT(right).value());
            if (pawn && pawn->GetColor() != m_color)
            {
                auto to = Coordinates(m_coordinates.GetRank() + offset, m_coordinates.GetFile());
                Util::Debugger::Debug(
                    "[PawnPiece::MakeMove] en passant right to %d,%d\n",
                    to.GetRank(),
                    to.GetFile()
                );
                pawn->m_en_passant_moves.push_back(Move(pawn->m_coordinates, to, m_coordinates));
            }
        }
    }
}

void PawnPiece::SetEnPassantMoves(std::vector<Move> moves)
{
    m_en_passant_moves = moves;
}
} // namespace Game
