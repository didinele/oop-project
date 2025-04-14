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
    [[nodiscard]] Color GetColor() const;

    [[nodiscard]] virtual std::vector<Move> GetPossibleMoves(Board &board) const = 0;
    [[nodiscard]] std::vector<Piece *> GetSeenBy(Board &board) const;

    void MakeMove(Board &board, Move move);

    virtual ~Piece();

  protected:
    Piece(Color color, Coordinates coords);

    // Extremely useful for implementing rook+bishok+queen. Given directional offsets, it will traverse
    // the board in that direction until it hits a piece or goes out of bounds.
    [[nodiscard]] std::vector<Move> GetNaiveMovesInDirections(Board &board, std::vector<std::array<short, 2>> offsets) const;

    Color m_Color;
    Coordinates m_Coordinates;
};

#define PIECE_TYPE(type)                                                                           \
    class type : public Piece                                                                      \
    {                                                                                              \
      public:                                                                                      \
        using Piece::Piece;                                                                        \
        std::vector<Move> GetPossibleMoves(Board &board) const override;                           \
    }

PIECE_TYPE(KnightPiece);
PIECE_TYPE(QueenPiece);
PIECE_TYPE(KingPiece);
} // namespace game
