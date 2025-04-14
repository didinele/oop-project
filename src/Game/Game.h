#pragma once

#include "Common.h"
#include "./Piece/Piece.h"
#include <optional>

namespace game
{
class Game
{
  public:
    Game();

    [[nodiscard]] std::optional<Color> GetCurrentPlayer() const;
    [[nodiscard]] GameState GetState() const;

  private:
    Color m_CurrentPlayer;
    GameState m_State;
    Board m_Board;
};
} // namespace game
