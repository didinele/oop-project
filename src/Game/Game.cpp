#include "Game.h"
#include "../Util/Debug.h"
#include "Piece/BishopPiece.h"
#include "Piece/KingPiece.h"
#include "Piece/KnightPiece.h"
#include "Piece/PawnPiece.h"
#include "Piece/Piece.h"
#include "Piece/QueenPiece.h"
#include "Piece/RookPiece.h"
#include <optional>

namespace game
{
[[nodiscard]] static std::array<Piece *, 8> get_pieces(Color color)
{
    std::array<Piece *, 8> pieces;

    auto rank = color == Color::White ? 0 : 7;

    pieces[0] = new RookPiece(color, Coordinates(rank, 0), RookKind::Long);
    pieces[1] = new KnightPiece(color, Coordinates(rank, 1));
    pieces[2] = new BishopPiece(color, Coordinates(rank, 2));
    pieces[3] = new QueenPiece(color, Coordinates(rank, 3));
    pieces[4] = new KingPiece(color, Coordinates(rank, 4));
    pieces[5] = new BishopPiece(color, Coordinates(rank, 5));
    pieces[6] = new KnightPiece(color, Coordinates(rank, 6));
    pieces[7] = new RookPiece(color, Coordinates(rank, 7), RookKind::Short);

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

Board Game::GetBoard() const
{
    return m_Board;
}

bool Game::MakeMove(Move move)
{
    // Considering <Piece>.MakeMove() does not ensure the king is not in check
    // after the move, the laziest way to do it is to always "simulate" moves
    // before committing them.
    auto clone = CloneBoard();
    auto piece = clone[move.from.GetRank()][move.from.GetFile()].value();
    piece->MakeMove(clone, move);

    // Find our king and make sure it is not in check
    KingPiece *king = nullptr;
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = clone[rank][file];
            if (!option.has_value())
            {
                continue;
            }

            auto piece = option.value();
            if (piece->GetColor() == m_CurrentPlayer)
            {
                auto found = dynamic_cast<KingPiece *>(piece);
                if (found != nullptr)
                {
                    king = found;
                    if (found->IsInCheck(clone))
                    {
                        util::Debugger::Debug("[Game::MakeMove] King is in check after move\n");
                        return false;
                    }
                }
            }
        }
    }

    m_Board = clone;
    operator[](move.from) = std::nullopt;
    operator[](move.to) = piece->Clone();
    m_State = king->mated ? GameState::Ended : GameState::Waiting;
    m_CurrentPlayer = m_CurrentPlayer == Color::White ? Color::Black : Color::White;

    // Check for stalemate
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = m_Board[rank][file];
            if (!option.has_value())
            {
                continue;
            }

            auto piece = option.value();
            if (piece->GetColor() == m_CurrentPlayer)
            {
                auto moves = piece->GetPossibleMoves(m_Board);
                if (!moves.empty())
                {
                    return true;
                }
            }
        }
    }

    // If we're here, we could not find any legal moves. This means we're in stalemate.
    m_State = GameState::Stalemate;

    return true;
}

std::optional<Piece *> Game::operator[](const Coordinates &coordinates)
{
    return m_Board[coordinates.GetRank()][coordinates.GetFile()];
}

Board Game::CloneBoard() const
{
    Board clone;
    for (auto rank = 0; rank < 8; rank++)
    {
        for (auto file = 0; file < 8; file++)
        {
            auto option = m_Board[rank][file];
            if (option.has_value())
            {
                clone[rank][file] = option.value()->Clone();
            }
            else
            {
                clone[rank][file] = std::nullopt;
            }
        }
    }
    return clone;
}
} // namespace game
