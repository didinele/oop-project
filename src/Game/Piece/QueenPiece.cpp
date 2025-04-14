#include "QueenPiece.h"
#include <vector>

namespace game
{
std::vector<Move> QueenPiece::GetPossibleMoves(Board &board) const
{
    return GetNaiveMovesInDirections(
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
} // namespace game
