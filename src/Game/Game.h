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
    Game(const Game &other) = delete;
    Game &operator=(const Game &other) = delete;
    ~Game();

    [[nodiscard]] Color GetCurrentPlayer() const;
    [[nodiscard]] GameState GetState() const;
    [[nodiscard]] Board GetBoard() const;

    void Draw();

    // Mostly delegates to <Piece>.MakeMove(), but adds some extra checks and maintains
    // game state.
    [[nodiscard]] bool MakeMove(Move move);

    [[nodiscard]] std::optional<Piece *> operator[](const Coordinates &coordinates);

  private:
    Color m_CurrentPlayer;
    GameState m_State;
    Board m_Board;
};
} // namespace game
