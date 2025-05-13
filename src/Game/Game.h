#pragma once

#include "./Piece/piece.h"
#include "coordinates.h"
#include <optional>

namespace Game
{
enum class GameState
{
    // Waiting for a move
    Waiting,
    // Game ended normally (clock/checkmate/resignation)
    Ended,
    // Game ended in a draw (note, stalemate checking does not exist. this is manual)
    Draw,
};

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

    void Resign();
    void Draw();

    // Mostly delegates to <Piece>.MakeMove(), but adds some extra checks and maintains
    // game state.
    [[nodiscard]] bool MakeMove(Move move);

    [[nodiscard]] std::optional<Piece *> operator[](const Coordinates &coordinates);

  private:
    Color m_current_player;
    GameState m_state;
    Board m_board;
};
} // namespace Game
