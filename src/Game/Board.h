#pragma once

#include "Common.h"
#include <vector>

namespace game
{
class Tile
{
  public:
    Tile(Coordinates coords) : m_Coordinates(coords)
    {
        std::optional<PieceKind> kind;
        Color color;

        static PieceKind pieces[8] = {
            PieceKind::Rook,
            PieceKind::Knight,
            PieceKind::Bishop,
            PieceKind::Queen,
            PieceKind::King,
            PieceKind::Bishop,
            PieceKind::Knight,
            PieceKind::Rook
        };

        auto file = static_cast<short>(coords.GetFile());

        if (coords.GetRank() == Rank::One)
        {
            color = Color::White;
            kind = pieces[file];
        }
        else if (coords.GetRank() == Rank::Eight)
        {
            color = Color::Black;
            kind = pieces[file];
        }
        else if (coords.GetRank() == Rank::Two)
        {
            color = Color::White;
            kind = PieceKind::Pawn;
        }
        else if (coords.GetRank() == Rank::Seven)
        {
            color = Color::Black;
            kind = PieceKind::Pawn;
        }

        if (kind.has_value())
        {
            m_Piece.emplace(*kind, color);
        }
    }

  private:
    Coordinates m_Coordinates;
    std::optional<Piece> m_Piece;
};

class GameBoard
{
  public:
    constexpr GameBoard()
    {
        for (int rank = 0; rank < 8; ++rank)
        {
            std::vector<Tile> vec;
            for (int file = 0; file < 8; ++file)
            {
                vec.push_back(Tile(Coordinates(static_cast<Rank>(rank), static_cast<File>(file))));
            }

            m_Tiles.push_back(vec);
        }
    }
    GameBoard(const GameBoard &) = delete;
    GameBoard &operator=(const GameBoard &) = delete;

  private:
    std::vector<std::vector<Tile>> m_Tiles;
};
} // namespace game
