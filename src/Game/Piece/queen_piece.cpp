#include "queen_piece.h"
#include <vector>

namespace Game
{
QueenPiece::QueenPiece(Color color, Coordinates coords) : Piece(color, coords)
{
}

std::vector<Move> QueenPiece::GetPossibleMoves(Board &board) const
{
    return _GetNaiveMovesInDirections(
        board,
        std::vector<std::array<short, 2>>{
            {1, -1},  // top left
            {1, 1},   // top right
            {-1, -1}, // bottom left
            {-1, 1},  // bottom right

            {1, 0},  // up
            {-1, 0}, // down
            {0, -1}, // left
            {0, 1},  // right
        }
    );
}
Piece *QueenPiece::Clone() const
{
    return new QueenPiece(m_color, m_coordinates);
}
} // namespace Game
