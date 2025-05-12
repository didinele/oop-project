#include "KingPiece.h"
#include "../../Util/Debug.h"
#include "RookPiece.h"
#include <cassert>
#include <vector>

static short offsets[8][2] = {{1, 0}, {1, 1}, {1, -1}, {-1, 0}, {-1, 1}, {-1, -1}, {0, 1}, {0, -1}};

namespace game
{
KingPiece::KingPiece(Color color, Coordinates coords) : Piece(color, coords)
{
}

std::vector<Move> KingPiece::GetPossibleMoves(Board &board) const
{
    std::vector<Move> out;

    if (CanCastleShort(board))
    {
        util::Debugger::Debug("[KingPiece::GetPossibleMoves] can castle short\n");
        auto to = m_Color == Color::White ? Coordinates(0, 6) : Coordinates(7, 6);
        out.push_back(Move(m_Coordinates, to, CastleKind::Short));
    }

    if (CanCastleLong(board))
    {
        util::Debugger::Debug("[KingPiece::GetPossibleMoves] can castle long\n");
        auto to = m_Color == Color::White ? Coordinates(0, 2) : Coordinates(7, 2);
        out.push_back(Move(m_Coordinates, to, CastleKind::Long));
    }

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
            // In the past, we checked `piece.value()->GetSeenBy(board).empty()`. That is infinitely
            // recursive (GetSeenBy calls GetPossibleMoves for all pieces, including this very one).
            // Let's just compromise and not check. Game will undo the move anyways if it puts the
            // king into check.
            if (piece.value()->GetColor() != m_Color)
            {
                out.push_back(Move(m_Coordinates, to));
            }
        }
        else
        {
            // Make sure the square isn't seen by an enemy piece
            bool seen = false;
            for (auto rank = 0; rank < 8 && !seen; rank++)
            {
                for (auto file = 0; file < 8; file++)
                {
                    auto option = board[rank][file];
                    if (option.has_value() && option.value()->GetColor() != m_Color)
                    {
                        // Prevent the classic infinite recursion
                        if (dynamic_cast<KingPiece *>(option.value()) != nullptr)
                        {
                            continue;
                        }

                        auto moves = option.value()->GetPossibleMoves(board);
                        for (auto &move : moves)
                        {
                            if (move.to == to)
                            {
                                seen = true;
                                break;
                            }
                        }
                    }
                }
            }

            if (!seen)
            {
                out.push_back(Move(m_Coordinates, to));
            }
        }
    }

    return out;
}

Piece *KingPiece::Clone() const
{
    auto other = new KingPiece(m_Color, m_Coordinates);
    other->m_HasMoved = m_HasMoved;
    other->m_IsMated = m_IsMated;

    return other;
}

bool KingPiece::GetIsMated() const
{
    return m_IsMated;
}

void KingPiece::SetMated()
{
    m_IsMated = true;
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
                // (white) KingPiece::GetPossibleMoves() -> KingPiece::CanCastleShort() ->
                // KingPiece::IsInCheck() -> (black) KingPiece::GetPossibleMoves() is a fatal stack
                // overflow infinite recursion. Easiest fix is to just check if the piece is a king
                // and skip it. Problem is, this skip makes it so kings can end up next to each
                // other. We account for this by checking if this king we just found is on an
                // adjacent square
                if (dynamic_cast<KingPiece *>(option.value()) != nullptr)
                {
                    auto king_coords = option.value()->GetCoordinates();
                    for (auto &offset : offsets)
                    {
                        auto rank_offset = offset[0];
                        auto file_offset = offset[1];

                        auto to = Coordinates(
                            king_coords.GetRank() + rank_offset,
                            king_coords.GetFile() + file_offset
                        );

                        if (to == m_Coordinates)
                        {
                            util::Debugger::Debug(
                                "[KingPiece::IsInCheck] king is next to enemy king\n"
                            );
                            return true;
                        }
                    }

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
    auto scope = util::Debugger::CreateScope("KingPiece::CanCastleShort");
    if (m_HasMoved || IsInCheck(board))
    {
        scope.Debug("king has already moved or is currently in check\n");
        return false;
    }

    // Check if the rook is available
    bool found = false;
    for (auto rank = 0; rank < 8 && !found; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (!option.has_value() || option.value()->GetColor() != m_Color)
            {
                continue;
            }

            auto piece = dynamic_cast<RookPiece *>(option.value());
            if (piece != nullptr && piece->GetKind() == RookKind::Short)
            {
                if (piece->GetHasMoved())
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
        scope.Debug("Rook not on the board or has already moved\n");
        return false;
    }

    // Collect all opponent pieces' available moves and check if anything can see
    // squares in our path
    Coordinates check_against[2] = {
        Coordinates(m_Color == Color::White ? 0 : 7, 5),
        Coordinates(m_Color == Color::White ? 0 : 7, 6),
    };

    for (auto &coords : check_against)
    {
        if (BOARD_AT(coords).has_value())
        {
            scope.Debug("Pieces in way of castling\n");
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
                // (white) KingPiece::GetPossibleMoves() -> KingPiece::CanCastleShort() ->
                // (black) KingPiece::GetPossibleMoves() is a fatal stack overflow infinite
                // recursion. Easiest fix is to just check if the piece is a king
                if (dynamic_cast<KingPiece *>(option.value()) != nullptr)
                {
                    continue;
                }

                auto moves = option.value()->GetPossibleMoves(board);

                for (auto &move : moves)
                {
                    for (auto &check : check_against)
                    {
                        if (move.to == check)
                        {
                            scope.Debug("Opponent piece can see square in path\n");
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
    auto scope = util::Debugger::CreateScope("KingPiece::CanCastleLong");
    if (m_HasMoved || IsInCheck(board))
    {
        scope.Debug("king has already moved or is currently in check\n");
        return false;
    }

    // Check if the rook is available
    bool found = false;
    for (auto rank = 0; rank < 8 && !found; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (!option.has_value() || option.value()->GetColor() != m_Color)
            {
                continue;
            }

            auto piece = dynamic_cast<RookPiece *>(option.value());
            if (piece != nullptr && piece->GetKind() == RookKind::Long)
            {
                if (piece->GetHasMoved())
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
        scope.Debug("Rook not on the board or has already moved\n");
        return false;
    }

    // Collect all opponent pieces' available moves and check if anything can see
    // squares in our path
    Coordinates check_against[3] = {
        Coordinates(m_Color == Color::White ? 0 : 7, 1),
        Coordinates(m_Color == Color::White ? 0 : 7, 2),
        Coordinates(m_Color == Color::White ? 0 : 7, 3),
    };

    for (auto &coords : check_against)
    {
        if (BOARD_AT(coords).has_value())
        {
            scope.Debug("Pieces in way of castling\n");
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

                // (white) KingPiece::GetPossibleMoves() -> KingPiece::CanCastleLong() ->
                // (black) KingPiece::GetPossibleMoves() is a fatal stack overflow infinite
                // recursion. Easiest fix is to just check if the piece is a king
                if (dynamic_cast<KingPiece *>(option.value()) != nullptr)
                {
                    continue;
                }

                for (auto &move : moves)
                {
                    for (auto &check : check_against)
                    {
                        if (move.to == check)
                        {
                            scope.Debug("Opponent piece can see square in path\n");
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

    // If we are castling, move the rook as well
    if (move.castleKind.has_value())
    {
        auto castle_kind = move.castleKind.value();
        auto rook_rank = m_Color == Color::White ? 0 : 7;
        auto rook_file = castle_kind == CastleKind::Short ? 7 : 0;

        auto rook = dynamic_cast<RookPiece *>(board[rook_rank][rook_file].value());
        assert(rook != nullptr);

        auto new_coords = Coordinates(rook_rank, castle_kind == CastleKind::Short ? 5 : 3);
        rook->SetCoordinates(new_coords);
        rook->SetMoved();
        BOARD_AT(new_coords) = rook;
        board[rook_rank][rook_file] = std::nullopt;
    }
}
} // namespace game
