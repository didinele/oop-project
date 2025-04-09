#pragma once

#include <format>
#include <string>

namespace game
{
enum class Color
{
    White,
    Black,
};

enum class GameState
{
    // Waiting for the first move/clock isn't yet ticking
    Waiting,
    // Waiting for a move, clock is ticking
    Ticking,
    // Game ended
    Ended,
};

enum class Rank
{
    One,
    Two,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
};

enum class File
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
};

enum class PieceKind
{
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
};

class Piece
{
  public:
    Piece(PieceKind kind, Color color) : m_Kind(kind), m_Color(color)
    {
    }

    PieceKind GetKind() const
    {
        return m_Kind;
    }
    Color GetColor() const
    {
        return m_Color;
    }

  private:
    PieceKind m_Kind;
    Color m_Color;
};

class Coordinates
{
  public:
    constexpr Coordinates(Rank rank, File file) : m_Rank(rank), m_File(file)
    {
    }
    Coordinates(const Coordinates &) = default;
    Coordinates &operator=(const Coordinates &) = default;

    Rank GetRank() const
    {
        return m_Rank;
    }
    File GetFile() const
    {
        return m_File;
    }

    Coordinates new_with_rank(Rank rank) const
    {
        return Coordinates(rank, m_File);
    }
    Coordinates new_with_file(File file) const
    {
        return Coordinates(m_Rank, file);
    }

    std::string ToString() const
    {
        static const char file_names[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
        static const char rank_names[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};

        auto file = file_names[static_cast<int>(m_File)];
        auto rank = rank_names[static_cast<int>(m_Rank)];
        return std::format("{}{}", file, rank);
    }

    bool operator==(const Coordinates &other) const
    {
        return m_Rank == other.m_Rank && m_File == other.m_File;
    }
    bool operator!=(const Coordinates &other) const
    {
        return !(*this == other);
    }

  private:
    Rank m_Rank;
    File m_File;
};
} // namespace game
