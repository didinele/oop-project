#pragma once

#include "./Piece/Piece.h"
#include "Common.h"
#include <optional>

namespace game
{
class Game
{
  public:
    Game();
    Game &operator=(const Game &other);
    ~Game();

    [[nodiscard]] std::optional<Color> GetCurrentPlayer() const;
    [[nodiscard]] GameState GetState() const;

    [[nodiscard]] std::optional<Piece *> operator[](const Coordinates &coordinates);

  private:
    Color m_CurrentPlayer;
    GameState m_State;
    Board m_Board;
};
} // namespace game
