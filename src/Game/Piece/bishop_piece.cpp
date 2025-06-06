#include "bishop_piece.h"
#include <vector>

namespace Game
{
BishopPiece::BishopPiece(Color color, Coordinates coords) : Piece(color, coords)
{
}

std::vector<Move> BishopPiece::GetPossibleMoves(Board &board) const
{
    return _GetNaiveMovesInDirections(
        board,
        std::vector<std::array<short, 2>>{
            {1, -1},  // top left
            {1, 1},   // top right
            {-1, -1}, // bottom left
            {-1, 1},  // bottom right
        }
    );
}

Piece *BishopPiece::Clone() const
{
    return new BishopPiece(m_color, m_coordinates);
}
} // namespace Game
