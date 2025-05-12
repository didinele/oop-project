#pragma once

#include "Coordinates.h"
#include <optional>
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

enum class CastleKind
{
    Long,
    Short
};

struct Move
{
    [[nodiscard]] static std::vector<Move> GetPromotionMoves(Coordinates from, Coordinates to);

    Move(Coordinates from, Coordinates to);
    Move(Coordinates from, Coordinates to, Coordinates passanted);
    Move(Coordinates from, Coordinates to, PromotionKind);
    Move(Coordinates from, Coordinates to, CastleKind);
    Move(Coordinates from, Coordinates to, PromotionKind, CastleKind);

    Coordinates from;
    Coordinates to;

    std::optional<Coordinates> passanted;
    std::optional<PromotionKind> promotionKind;
    std::optional<CastleKind> castleKind;
};
} // namespace game
