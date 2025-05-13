#pragma once

#include <string>

namespace game
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
    short m_Rank;
    short m_File;
};
} // namespace game
