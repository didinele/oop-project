#include "piece.h"
#include "../../Util/debug.h"
#include "bishop_piece.h"
#include "king_piece.h"
#include "knight_piece.h"
#include "pawn_piece.h"
#include "queen_piece.h"
#include "rook_piece.h"
#include <cassert>
#include <optional>

namespace Game
{
Piece::Piece(Color color, Coordinates coords) : m_color(color), m_coordinates(coords)
{
}
Piece::~Piece() = default;
Coordinates Piece::GetCoordinates() const
{
    return m_coordinates;
}
void Piece::SetCoordinates(Coordinates coords)
{
    m_coordinates = coords;
}
Color Piece::GetColor() const
{
    return m_color;
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
            if (piece->GetColor() == m_color)
            {
                continue;
            }

            auto moves = piece->GetPossibleMoves(board);
            for (auto &move : moves)
            {
                // Edge case with pawns preventing certain things by "seeing" above them
                if (dynamic_cast<PawnPiece *>(piece) != nullptr &&
                    move.to.GetRank() == m_coordinates.GetRank())
                {
                    continue;
                }

                if (move.to == m_coordinates)
                {
                    out.push_back(piece);
                }
            }
        }
    }

    return out;
}

void Piece::_MakeMove(Board &board, Move move, bool simulate)
{
    auto scope = Util::Debugger::CreateScope("Piece::MakeMove");

    auto &to = BOARD_AT(move.to);

    // Free the memory for the captured piece (if any)
    if (to.has_value())
    {
        scope.Debug("Captured piece at %s\n", move.to.ToString().c_str());
        auto piece = to.value();
        delete piece;
    }

    if (move.passanted.has_value())
    {
        scope.Debug("Captured en passant at %s\n", move.passanted.value().ToString().c_str());
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
            if (piece->GetColor() == m_color)
            {
                auto pawn = dynamic_cast<PawnPiece *>(piece);
                if (pawn != nullptr)
                {
                    pawn->SetEnPassantMoves(std::vector<Move>());
                }
            }

            if (enemy_king == nullptr && piece->GetColor() != m_color)
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
        scope.Debug("King is in check and has empty GetPossibleMoves\n");
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
                for (auto &test_move : possible_moves)
                {
                    // Step 2. Now that we have possible moves, we need to check if any of them
                    // can escape the check. To simplify our logic, despite the perhaps poor
                    // performance, we're just gonna deep clone the board and run the move on it
                    // arbitrarily.
                    auto temp_board = CloneBoard(board);

                    auto piece =
                        temp_board[test_move.from.GetRank()][test_move.from.GetFile()].value();
                    piece->_MakeMove(temp_board, test_move, true);

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
            scope.Debug("King is mated\n");
            enemy_king->SetMated();
        }
    }
}

std::vector<Move>
Piece::_GetNaiveMovesInDirections(Board &board, std::vector<std::array<short, 2>> offsets) const
{
    std::vector<Move> out;

    for (auto &offset : offsets)
    {
        auto rank_offset = offset[0];
        auto file_offset = offset[1];

        while (true)
        {
            auto to = Coordinates(
                m_coordinates.GetRank() + rank_offset,
                m_coordinates.GetFile() + file_offset
            );
            if (!to.IsValid())
            {
                break;
            }

            auto piece = BOARD_AT(to);
            if (piece.has_value())
            {
                if (piece.value()->GetColor() != m_color)
                {
                    out.push_back(Move(m_coordinates, to));
                }

                break;
            }

            out.push_back(Move(m_coordinates, to));

            rank_offset += offset[0];
            file_offset += offset[1];
        }
    }

    return out;
}

Board CloneBoard(Board &board)
{
    Board clone;
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (option.has_value())
            {
                clone[rank][file] = option.value()->Clone();
            }
            else
            {
                clone[rank][file] = std::nullopt;
            }
        }
    }
    return clone;
}

void FreeBoard(Board &board)
{
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = board[rank][file];
            if (option.has_value())
            {
                delete option.value();
                board[rank][file] = std::nullopt;
            }
        }
    }
}
} // namespace Game
