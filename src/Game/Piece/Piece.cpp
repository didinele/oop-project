#include "Piece.h"
#include "BishopPiece.h"
#include "KnightPiece.h"
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
void Piece::MakeMove(Board &board, Move move)
{
    auto &to = BOARD_AT(move.to);

    // Free the memory for the captured piece (if any)
    if (to.has_value())
    {
        auto piece = to.value();
        delete piece;
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
        to = BOARD_AT(move.from);
    }

    BOARD_AT(move.from) = std::nullopt;
}

std::vector<Move>
Piece::GetNaiveMovesInDirections(Board &board, std::vector<std::array<short, 2>> offsets) const
{
    std::vector<Move> out;

    for (auto &offset : offsets)
    {
        auto rankOffset = offset[0];
        auto fileOffset = offset[1];

        while (true)
        {
            auto to = Coordinates(
                m_Coordinates.GetRank() + rankOffset,
                m_Coordinates.GetFile() - fileOffset
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

            rankOffset += offset[0];
            fileOffset += offset[1];
        }
    }

    return out;
}
} // namespace game
