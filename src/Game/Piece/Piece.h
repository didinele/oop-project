#pragma once

#include "../Common.h"
#include "../Move.h"
#include <vector>

#define Board std::array<std::array<std::optional<Piece *>, 8>, 8>
#define BOARD_AT(coords) board[coords.GetRank()][coords.GetFile()]

namespace game
{

class Piece
{
  public:
    [[nodiscard]] Coordinates GetCoordinates() const;
    void SetCoordinates(Coordinates coords);
    [[nodiscard]] Color GetColor() const;

    // Gets moves this piece can make, without accounting for much game state.
    // For instance, this will still return moves that would put the king in check
    // Requiring a tad bit more validation before calling MakeMove
    [[nodiscard]] virtual std::vector<Move> GetPossibleMoves(Board &board) const = 0;
    [[nodiscard]] virtual Piece *Clone() const = 0;
    [[nodiscard]] std::vector<Piece *> GetSeenBy(Board &board) const;

    // Generally, moves returned by `GetPossibleMoves` can be safely passed here.
    // !! The only danger is that this does not check if the move would put the king in check !!
    // This also performs some clean-up and game state updates (king is mated, in check, etc)
    // Due to the complexity of checking for mate, we sometimes require to "simulate" moves on a
    // deep-copied board. As such, the protected `MakeMove` overload with the extra bool is what
    // contains the true implementation. Some child pieces override that method to perform
    // additional updates related to their own state
    void MakeMove(Board &board, Move move)
    {
        MakeMove(board, move, false);
    }

    virtual ~Piece();

  protected:
    Piece(Color color, Coordinates coords);

    // Extremely useful for implementing rook+bishok+queen. Given directional offsets, it will
    // traverse the board in that direction until it hits a piece or goes out of bounds.
    [[nodiscard]] std::vector<Move>
    GetNaiveMovesInDirections(Board &board, std::vector<std::array<short, 2>> offsets) const;

    virtual void MakeMove(Board &board, Move move, bool simulate);

    Color m_Color;
    Coordinates m_Coordinates;
};
} // namespace game
