#include "Game.h"
#include <optional>

namespace game
{
Game::Game() : m_CurrentPlayer(Color::White), m_State(GameState::Waiting)
{
}

std::optional<Color> Game::GetCurrentPlayer() const
{
    return m_CurrentPlayer;
}

GameState Game::GetState() const
{
    return m_State;
}
} // namespace game
