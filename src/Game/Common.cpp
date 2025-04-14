#include "Common.h"
#include "format"

namespace game
{
Coordinates::Coordinates(short rank, short file) : m_Rank(rank), m_File(file)
{
}

short Coordinates::GetRank() const
{
    return m_Rank;
}
short Coordinates::GetFile() const
{
    return m_File;
}

Coordinates Coordinates::NewWithRank(short rank) const
{
    return Coordinates(rank, m_File);
}
Coordinates Coordinates::NewWithFile(short file) const
{
    return Coordinates(m_Rank, file);
}

std::string Coordinates::ToString() const
{
    static const char file_names[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    static const char rank_names[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};

    auto file = file_names[m_File];
    auto rank = rank_names[m_Rank];
    return std::format("{}{}", file, rank);
}

bool Coordinates::IsValid() const
{
    return m_Rank >= 0 && m_Rank < 8 && m_File >= 0 && m_File < 8;
}

bool Coordinates::IsPromotionSquare(Color color) const
{
    return color == Color::White ? (m_Rank == 7) : (m_Rank == 0);
}

bool Coordinates::operator==(const Coordinates &other) const
{
    return m_Rank == other.m_Rank && m_File == other.m_File;
}
bool Coordinates::operator!=(const Coordinates &other) const
{
    return !(*this == other);
}
} // namespace game
