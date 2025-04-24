#include "Piece.h"
#include "BishopPiece.h"
#include "KingPiece.h"
#include "KnightPiece.h"
#include "PawnPiece.h"
#include "QueenPiece.h"
#include "RookPiece.h"
#include <cassert>
#include <optional>

namespace game
{
Piece::Piece(Color m_Color, Coordinates coords) : m_Color(m_Color), m_Coordinates(coords)
{
}
Piece::~Piece() = default;
Coordinates Piece::GetCoordinates() const
{
    return m_Coordinates;
}
void Piece::SetCoordinates(Coordinates coords)
{
    m_Coordinates = coords;
}
Color Piece::GetColor() const
{
    return m_Color;
}
std::vector<Piece *> Piece::GetSeenBy(Board &board) const
{
    std::vector<Piece *> out;

    for (auto i = 0; i < 8; i++)
    {
        for (auto j = 0; j < 8; j++)
        {
            auto option = board[i][j];
            if (!option.has_value())
            {
                continue;
            }

            auto piece = option.value();
            if (piece->GetColor() == m_Color)
            {
                continue;
            }

            auto moves = piece->GetPossibleMoves(board);
            for (auto &move : moves)
            {
                if (move.to == m_Coordinates)
                {
                    out.push_back(piece);
                }
            }
        }
    }

    return out;
}

void Piece::MakeMove(Board &board, Move move, bool simulate)
{
    auto &to = BOARD_AT(move.to);

    // Free the memory for the captured piece (if any)
    if (to.has_value())
    {
        auto piece = to.value();
        delete piece;
    }

    if (move.passanted.has_value())
    {
        auto piece = BOARD_AT(move.passanted.value()).value();
        delete piece;
        BOARD_AT(move.passanted.value()) = std::nullopt;
    }

    // Actually make the move by swapping out the pointers
    if (move.promotionKind.has_value())
    {
        auto from = BOARD_AT(move.from).value();
        auto kind = move.promotionKind.value();
        Piece *new_piece = nullptr;

        switch (kind)
        {
            case PromotionKind::Bishop: {
                new_piece = new BishopPiece(from->GetColor(), move.to);
                break;
            }
            case PromotionKind::Knight: {
                new_piece = new KnightPiece(from->GetColor(), move.to);
                break;
            }
            case PromotionKind::Rook: {
                new_piece = new RookPiece(from->GetColor(), move.to);
                break;
            }
            case PromotionKind::Queen: {
                new_piece = new QueenPiece(from->GetColor(), move.to);
                break;
            }
        }

        assert(new_piece != nullptr);

        // Free the old piece
        delete from;
        to = std::make_optional(new_piece);
    }
    else
    {
        BOARD_AT(move.from).value()->SetCoordinates(move.to);
        to = BOARD_AT(move.from);
    }

    BOARD_AT(move.from) = std::nullopt;

    if (simulate)
    {
        return;
    }

    // Time for some game-state cleanup. To avoid looping twice, this loop will also serve to find
    // our kings
    KingPiece *enemy_king = nullptr;
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (!option.has_value())
            {
                continue;
            }

            auto piece = option.value();
            // For one, given a move was made, all pawns of the current color lose the en passant
            // state (if they had it)
            if (piece->GetColor() == m_Color)
            {
                auto pawn = dynamic_cast<PawnPiece *>(piece);
                if (pawn != nullptr)
                {
                    pawn->enPassantMoves = std::vector<Move>();
                }
            }

            if (enemy_king == nullptr && piece->GetColor() != m_Color)
            {
                enemy_king = dynamic_cast<KingPiece *>(piece);
            }
        }
    }

    assert(enemy_king != nullptr);

    // Check for mate
    // Step 1. King is in check and has no legal moves
    if (enemy_king->IsInCheck(board) && enemy_king->GetPossibleMoves(board).empty())
    {
        bool can_escape = false;

        // Now, let's look for all the legal moves of the opponent's piece.
        for (auto rank = 0; rank < 8 && !can_escape; rank++)
        {
            for (auto file = 0; file < 8 && !can_escape; file++)
            {
                auto option = board[rank][file];
                if (!option.has_value() || option.value()->GetColor() != enemy_king->GetColor())
                {
                    continue;
                }

                auto possible_moves = option.value()->GetPossibleMoves(board);
                for (const auto &test_move : possible_moves)
                {
                    // Step 2. Now that we have possible moves, we need to check if any of them
                    // can escape the check. To simplify our logic, despite the perhaps poor
                    // performance, we're just gonna deep clone the board and run the move on it
                    // arbitrarily.
                    Board temp_board = board;

                    auto piece =
                        temp_board[test_move.from.GetRank()][test_move.from.GetFile()].value();
                    piece->MakeMove(temp_board, test_move, true);

                    // Step 3. Check if the king is still in check after the move
                    // First, we need to find the king on the copied board. Given
                    // GetPossibleMoves(board).empty() was true, we know the king is in the same
                    // spot
                    auto temp_enemy_king =
                        dynamic_cast<KingPiece *>(temp_board[enemy_king->GetCoordinates().GetRank()]
                                                            [enemy_king->GetCoordinates().GetFile()]
                                                                .value());

                    // Check if the king is still in check after the simulated move
                    if (!temp_enemy_king->IsInCheck(temp_board))
                    {
                        // Found a legal move to escape check
                        can_escape = true;
                        break;
                    }

                    // Temp board is freed here
                }
            }
        }

        if (!can_escape)
        {
            enemy_king->mated = true;
        }
    }
}

std::vector<Move>
Piece::GetNaiveMovesInDirections(Board &board, std::vector<std::array<short, 2>> offsets) const
{
    std::vector<Move> out;

    for (auto &offset : offsets)
    {
        auto rank_offset = offset[0];
        auto file_offset = offset[1];

        while (true)
        {
            auto to = Coordinates(
                m_Coordinates.GetRank() + rank_offset,
                m_Coordinates.GetFile() + file_offset
            );
            if (!to.IsValid())
            {
                break;
            }

            auto piece = BOARD_AT(to);
            if (piece.has_value())
            {
                if (piece.value()->GetColor() != m_Color)
                {
                    out.push_back(Move(m_Coordinates, to));
                }

                break;
            }

            out.push_back(Move(m_Coordinates, to));

            rank_offset += offset[0];
            file_offset += offset[1];
        }
    }

    return out;
}
} // namespace game
