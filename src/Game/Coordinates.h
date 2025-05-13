#pragma once

#include <string>

namespace Game
{
enum class Color
{
    White,
    Black,
};

// TODO: + overloads
class Coordinates
{
  public:
    Coordinates(short rank, short file);

    [[nodiscard]] short GetRank() const;
    [[nodiscard]] short GetFile() const;

    [[nodiscard]] Coordinates NewWithRank(short rank) const;
    [[nodiscard]] Coordinates NewWithFile(short file) const;

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] bool IsPromotionSquare(Color color) const;

    bool operator==(const Coordinates &other) const;
    bool operator!=(const Coordinates &other) const;

  private:
    short m_rank;
    short m_file;
};
} // namespace Game
