#pragma once

#include "Common.h"
#include <vector>

namespace game
{
enum class PromotionKind
{
    Knight,
    Bishop,
    Rook,
    Queen,
};

class Move
{
  public:
    static std::vector<Move> GetPromotionMoves(Coordinates from, Coordinates to);

    Move(Coordinates from, Coordinates to);
    Move(Coordinates from, Coordinates to, std::optional<PromotionKind> promotionKind);

    Coordinates from;
    Coordinates to;
    std::optional<PromotionKind> promotionKind;
};
} // namespace game
