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

enum class CastleKind {
  Long,
  Short
};

class Move
{
  public:
    static std::vector<Move> GetPromotionMoves(Coordinates from, Coordinates to);

    Move(Coordinates from, Coordinates to);
    Move(Coordinates from, Coordinates to, PromotionKind);
    Move(Coordinates from, Coordinates to, CastleKind);
    Move(Coordinates from, Coordinates to, PromotionKind, CastleKind);

    Coordinates from;
    Coordinates to;

    std::optional<PromotionKind> promotionKind;
    std::optional<CastleKind> castleKind;
};
} // namespace game
