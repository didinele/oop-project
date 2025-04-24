#include "Move.h"

namespace game
{
std::vector<Move> Move::GetPromotionMoves(Coordinates from, Coordinates to)
{
    std::vector<Move> out;

    out.push_back(Move(from, to, PromotionKind::Knight));
    out.push_back(Move(from, to, PromotionKind::Bishop));
    out.push_back(Move(from, to, PromotionKind::Rook));
    out.push_back(Move(from, to, PromotionKind::Queen));

    return out;
}

Move::Move(Coordinates from, Coordinates to) : from(from), to(to)
{
}
Move::Move(Coordinates from, Coordinates to, Coordinates passanted)
    : from(from), to(to), passanted(passanted)
{
}
Move::Move(Coordinates from, Coordinates to, PromotionKind promotionKind)
    : from(from), to(to), promotionKind(promotionKind)
{
}
Move::Move(Coordinates from, Coordinates to, CastleKind castleKind)
    : from(from), to(to), castleKind(castleKind)
{
}
Move::Move(Coordinates from, Coordinates to, PromotionKind promotionKind, CastleKind castleKind)
    : from(from), to(to), promotionKind(promotionKind), castleKind(castleKind)
{
}
} // namespace game
