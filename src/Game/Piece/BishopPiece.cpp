#include "BishopPiece.h"
#include <vector>

namespace game
{
std::vector<Move> BishopPiece::GetPossibleMoves(Board &board) const
{
    return GetNaiveMovesInDirections(board, std::vector<std::array<short, 2>>{
        {1, -1},  // top left
        {1, 1},   // top right
        {-1, -1}, // bottom left
        {-1, 1},  // bottom right
    });
}
} // namespace game
