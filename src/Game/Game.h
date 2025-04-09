#pragma once

#include "Board.h"
#include "Common.h"
#include <optional>

namespace game
{
class Game
{
  public:
    Game() : m_CurrentPlayer(Color::White), m_State(GameState::Waiting)
    {
    }

    std::optional<Color> GetCurrentPlayer() const
    {
        return m_CurrentPlayer;
    }
    GameState GetState() const
    {
        return m_State;
    }

  private:
    Color m_CurrentPlayer;
    GameState m_State;
    game::GameBoard m_Board;
};
} // namespace game
