#include "coordinates.h"
#include "format"

namespace Game
{
Coordinates::Coordinates(short rank, short file) : m_rank(rank), m_file(file)
{
}

short Coordinates::GetRank() const
{
    return m_rank;
}
short Coordinates::GetFile() const
{
    return m_file;
}

Coordinates Coordinates::NewWithRank(short rank) const
{
    return Coordinates(rank, m_file);
}
Coordinates Coordinates::NewWithFile(short file) const
{
    return Coordinates(m_rank, file);
}

std::string Coordinates::ToString() const
{
    static const char file_names[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    static const char rank_names[8] = {'1', '2', '3', '4', '5', '6', '7', '8'};

    auto file = file_names[m_file];
    auto rank = rank_names[m_rank];
    return std::format("{}{}", file, rank);
}

bool Coordinates::IsValid() const
{
    return m_rank >= 0 && m_rank < 8 && m_file >= 0 && m_file < 8;
}

bool Coordinates::IsPromotionSquare(Color color) const
{
    return color == Color::White ? (m_rank == 7) : (m_rank == 0);
}

bool Coordinates::operator==(const Coordinates &other) const
{
    return m_rank == other.m_rank && m_file == other.m_file;
}
bool Coordinates::operator!=(const Coordinates &other) const
{
    return !(*this == other);
}
} // namespace Game
