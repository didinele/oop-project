#include "RookPiece.h"

namespace game
{
std::vector<Move> RookPiece::GetPossibleMoves(Board &board) const
{
    return GetNaiveMovesInDirections(board, std::vector<std::array<short, 2>>{
        {1, 0},   // up
        {-1, 0},  // down
        {0, -1},  // left
        {0, 1},   // right
    });
}
} // namespace game
