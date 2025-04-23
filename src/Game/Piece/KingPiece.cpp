#include "KingPiece.h"
#include "RookPiece.h"
#include <vector>

namespace game
{
std::vector<Move> KingPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> out;

    if (CanCastleShort(board))
    {
        auto to = m_Color == Color::White ? Coordinates(0, 2) : Coordinates(7, 2);
        out.push_back(Move(m_Coordinates, to, CastleKind::Short));
    }

    if (CanCastleLong(board))
    {
        auto to = m_Color == Color::White ? Coordinates(0, 6) : Coordinates(7, 6);
        out.push_back(Move(m_Coordinates, to, CastleKind::Long));
    }

    short offsets[8][2] = {{1, 0}, {1, 1}, {1, -1}, {-1, 0}, {-1, 1}, {-1, -1}, {0, 1}, {0, -1}};

    for (auto &offset : offsets)
    {
        auto rank_offset = offset[0];
        auto file_offset = offset[1];

        auto to = Coordinates(
            m_Coordinates.GetRank() + rank_offset,
            m_Coordinates.GetFile() - file_offset
        );

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

bool KingPiece::IsInCheck(Board &board) const
{
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (option.has_value() && option.value()->GetColor() != m_Color)
            {
                // (white) KingPiece::GetPossibleMoves() -> KingPiece::CanCastleShort() -> KingPiece::IsInCheck() -> 
                // (black) KingPiece::GetPossibleMoves()
                // is a fatal stack overflow infinite recursion. Easiest fix is to
                // just check if the piece is a king and skip it; since kings can't cause checks anyway
                if (dynamic_cast<KingPiece *>(option.value()) != nullptr)
                {
                    continue;
                }

                auto moves = option.value()->GetPossibleMoves(board);
                for (auto &move : moves)
                {
                    if (move.to == m_Coordinates)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool KingPiece::CanCastleShort(Board &board) const
{
    if (m_HasMoved || IsInCheck(board))
    {
        return false;
    }

    // Check if the rook is available
    bool found = false;
    for (auto rank = 0; rank < 8 && !found; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (option.has_value() && option.value()->GetColor() != m_Color)
            {
                continue;
            }

            auto piece = dynamic_cast<RookPiece *>(option.value());
            if (piece != nullptr && piece->GetKind() == RookKind::Short)
            {
                if (piece->moved)
                {
                    return false;
                }

                found = true;
                break;
            }
        }
    }

    if (!found)
    {
        return false;
    }

    // Collect all opponent pieces' available moves and check if anything can see
    // squares in our path
    Coordinates check_against[2] = {
        Coordinates(m_Color == Color::White ? 0 : 7, 1),
        Coordinates(m_Color == Color::White ? 0 : 7, 2),
    };

    for (auto &coords : check_against)
    {
        if (BOARD_AT(coords).has_value())
        {
            return false;
        }
    }

    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (option.has_value() && option.value()->GetColor() != m_Color)
            {
                auto moves = option.value()->GetPossibleMoves(board);
                for (auto &move : moves)
                {
                    for (auto &check : check_against)
                    {
                        if (move.to == check)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool KingPiece::CanCastleLong(Board &board) const
{
    if (m_HasMoved || IsInCheck(board))
    {
        return false;
    }

    // Check if the rook is available
    bool found = false;
    for (auto rank = 0; rank < 8 && !found; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (option.has_value() && option.value()->GetColor() != m_Color)
            {
                continue;
            }

            auto piece = dynamic_cast<RookPiece *>(option.value());
            if (piece != nullptr && piece->GetKind() == RookKind::Long)
            {
                if (piece->moved)
                {
                    return false;
                }

                found = true;
                break;
            }
        }
    }

    if (!found)
    {
        return false;
    }

    // Collect all opponent pieces' available moves and check if anything can see
    // squares in our path
    Coordinates check_against[2] = {
        Coordinates(m_Color == Color::White ? 0 : 7, 4),
        Coordinates(m_Color == Color::White ? 0 : 7, 5),
    };

    for (auto &coords : check_against)
    {
        if (BOARD_AT(coords).has_value())
        {
            return false;
        }
    }

    if (board[0][6].has_value())
    {
        return false;
    }

    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (option.has_value() && option.value()->GetColor() != m_Color)
            {
                auto moves = option.value()->GetPossibleMoves(board);
                for (auto &move : moves)
                {
                    for (auto &check : check_against)
                    {
                        if (move.to == check)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

void KingPiece::MakeMove(Board &board, Move move, bool simulate)
{
    Piece::MakeMove(board, move, simulate);
    if (!simulate)
    {
        m_HasMoved = true;
    }
}
Piece *KingPiece::Clone() const
{
    return new KingPiece(m_Color, m_Coordinates);
}
} // namespace game
