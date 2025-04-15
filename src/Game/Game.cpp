#include "Game.h"
#include "Piece/BishopPiece.h"
#include "Piece/KingPiece.h"
#include "Piece/KnightPiece.h"
#include "Piece/PawnPiece.h"
#include "Piece/QueenPiece.h"
#include "Piece/RookPiece.h"
#include <optional>

namespace game
{
[[nodiscard]] static std::array<Piece *, 8> get_pieces(Color color)
{
    std::array<Piece *, 8> pieces;

    pieces[0] = new RookPiece(color, Coordinates(0, 0), RookKind::Short);
    pieces[1] = new KnightPiece(color, Coordinates(0, 1));
    pieces[2] = new BishopPiece(color, Coordinates(0, 2));
    pieces[3] = new QueenPiece(color, Coordinates(0, 3));
    pieces[4] = new KingPiece(color, Coordinates(0, 4));
    pieces[5] = new BishopPiece(color, Coordinates(0, 5));
    pieces[6] = new KnightPiece(color, Coordinates(0, 6));
    pieces[7] = new RookPiece(color, Coordinates(0, 7), RookKind::Long);

    return pieces;
}

Game::Game() : m_CurrentPlayer(Color::White), m_State(GameState::Waiting)
{
    auto white_pieces = get_pieces(Color::White);
    for (auto file = 0; file < 8; file++)
    {
        m_Board[0][file] = white_pieces[file];
        m_Board[1][file] = new PawnPiece(Color::White, Coordinates(1, file));
    }
    auto black_pieces = get_pieces(Color::Black);
    for (auto file = 0; file < 8; file++)
    {
        m_Board[7][file] = black_pieces[file];
        m_Board[6][file] = new PawnPiece(Color::Black, Coordinates(6, file));
    }
}

Game& Game::operator=(const Game &other)
{
    if (this != &other)
    {
        m_CurrentPlayer = other.m_CurrentPlayer;
        m_State = other.m_State;

        for (auto rank = 0; rank < 8; rank++)
        {
            for (auto file = 0; file < 8; file++)
            {
                auto piece = other.m_Board[rank][file];
                if (piece.has_value())
                {
                    m_Board[rank][file] = piece.value();
                }
                else
                {
                    m_Board[rank][file] = std::nullopt;
                }
            }
        }
    }

    return *this;
}

Game::~Game()
{
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto piece = m_Board[rank][file];
            if (piece.has_value())
            {
                delete piece.value();
                m_Board[rank][file] = std::nullopt;
            }
        }
    }
}

std::optional<Color> Game::GetCurrentPlayer() const
{
    return m_CurrentPlayer;
}

GameState Game::GetState() const
{
    return m_State;
}

std::optional<Piece *> Game::operator[](const Coordinates &coordinates)
{
    return m_Board[coordinates.GetRank()][coordinates.GetFile()];
}
} // namespace game
