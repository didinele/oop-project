#pragma once

#include <string>

namespace Game
{
enum class Color
{
    White,
    Black,
};

struct RankOffset
{
   short value;
};
struct FileOffset
{
    short value;
};

class Coordinates
{
  public:
    Coordinates(short rank, short file);

    [[nodiscard]] short GetRank() const;
    [[nodiscard]] short GetFile() const;

    [[nodiscard]] std::string ToString() const;

    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] bool IsPromotionSquare(Color color) const;

    bool operator==(const Coordinates &other) const;
    bool operator!=(const Coordinates &other) const;

    [[nodiscard]] Coordinates operator+(const Coordinates &other) const;
    [[nodiscard]] Coordinates operator+(const RankOffset &rank) const;
    [[nodiscard]] Coordinates operator+(const FileOffset &file) const;

  private:
    short m_rank;
    short m_file;
};
} // namespace Game
