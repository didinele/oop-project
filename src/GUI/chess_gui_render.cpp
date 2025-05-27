#include "../Game/Piece/bishop_piece.h"
#include "../Game/Piece/king_piece.h"
#include "../Game/Piece/knight_piece.h"
#include "../Game/Piece/pawn_piece.h"
#include "../Game/Piece/piece.h"
#include "../Game/Piece/queen_piece.h"
#include "../Game/Piece/rook_piece.h"
#include "../Util/debug.h"
#include "chess_gui.h"
#include <GLFW/glfw3.h>
#include <algorithm>

namespace GUI
{
void ChessGUI::_DrawBoard(ImDrawList &draw_list)
{
    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            auto in_light_square = (rank + file) % 2 != 0;
            ImVec4 square_color = in_light_square ? m_light_square_color : m_dark_square_color;

            Game::Coordinates coords(rank, file);
            ImVec2 p_min = _GetScreenPos(coords);
            ImVec2 p_max = ImVec2(p_min.x + m_square_size, p_min.y + m_square_size);

            draw_list.AddRectFilled(p_min, p_max, ImGui::ColorConvertFloat4ToU32(square_color));
        }
    }
}

void ChessGUI::_DrawPieces(ImDrawList &drawList)
{
    if (m_pieces_texture_id == 0 || m_texture_width == 0 || m_texture_height == 0)
    {
        return;
    }

    // Width of one piece in texture
    const auto piece_tex_width = static_cast<float>(m_texture_width) / 6.0f;
    // Height of one piece in texture
    const auto piece_tex_height = static_cast<float>(m_texture_height) / 2.0f;

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            auto coords = Game::Coordinates(rank, file);
            if (m_game->operator[](coords).has_value())
            {
                auto piece = m_game->operator[](coords).value();

                // Column index in the texture (0-5)
                int col = -1;
                // Row index in the texture atlas (0=white, 1=black)
                int row = -1;

                if (dynamic_cast<const Game::KingPiece *>(piece))
                    col = 0;
                else if (dynamic_cast<const Game::QueenPiece *>(piece))
                    col = 1;
                else if (dynamic_cast<const Game::BishopPiece *>(piece))
                    col = 2;
                else if (dynamic_cast<const Game::KnightPiece *>(piece))
                    col = 3;
                else if (dynamic_cast<const Game::RookPiece *>(piece))
                    col = 4;
                else if (dynamic_cast<const Game::PawnPiece *>(piece))
                    col = 5;

                if (piece->GetColor() == Game::Color::White)
                    row = 0;
                else if (piece->GetColor() == Game::Color::Black)
                    row = 1;

                if (col != -1 && row != -1)
                {
                    // Calculate coordinates
                    auto u0 = (col * piece_tex_width) / m_texture_width;
                    auto v0 = (row * piece_tex_height) / m_texture_height;
                    auto u1 = ((col + 1) * piece_tex_width) / m_texture_width;
                    auto v1 = ((row + 1) * piece_tex_height) / m_texture_height;

                    // Get screen position for the square
                    ImVec2 p_min = _GetScreenPos(coords);
                    ImVec2 p_max = ImVec2(p_min.x + m_square_size, p_min.y + m_square_size);

                    drawList.AddImage(
                        m_pieces_texture_id,
                        p_min,
                        p_max,
                        ImVec2(u0, v0),
                        ImVec2(u1, v1)
                    );
                }
            }
        }
    }
}

void GUI::ChessGUI::_DrawHighlights(ImDrawList &draw_list)
{
    // Draw highlight for selected square
    if (m_selected_square.has_value())
    {
        Game::Coordinates coords = m_selected_square.value();
        ImVec2 p_min = _GetScreenPos(coords);
        ImVec2 p_max = ImVec2(p_min.x + m_square_size, p_min.y + m_square_size);

        draw_list.AddRectFilled(p_min, p_max, ImGui::ColorConvertFloat4ToU32(m_highlight_color));

        // Draw highlights for possible moves
        for (auto &move : m_possible_moves_for_selected)
        {
            ImVec2 move_p_min = _GetScreenPos(move.to);
            // Draw a circle for possible moves
            draw_list.AddCircleFilled(
                ImVec2(move_p_min.x + m_square_size * 0.5f, move_p_min.y + m_square_size * 0.5f),
                m_square_size * 0.2f,
                ImGui::ColorConvertFloat4ToU32(m_highlight_color)
            );
        }
    }
}

void ChessGUI::_DrawPromotionDialog()
{
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::Begin("Promote Pawn", &m_promotion_dialog_active, flags))
    {
        ImGui::Text("Choose a piece for promotion:");

        const char *promotion_options[] = {"Queen", "Rook", "Bishop", "Knight"};
        // Default to Queen
        static int current_promotion = 0;

        ImGui::ListBox(
            "##PromotionSelection",
            &current_promotion,
            promotion_options,
            IM_ARRAYSIZE(promotion_options),
            4
        );

        if (ImGui::Button("Confirm", ImVec2(120, 0)))
        {
            // Apply the selected promotion
            Game::PromotionKind promotion_kind;

            switch (current_promotion)
            {
                case 0: {
                    promotion_kind = Game::PromotionKind::Queen;
                    break;
                }
                case 1: {
                    promotion_kind = Game::PromotionKind::Rook;
                    break;
                }
                case 2: {
                    promotion_kind = Game::PromotionKind::Bishop;
                    break;
                }
                case 3: {
                    promotion_kind = Game::PromotionKind::Knight;
                    break;
                }
                default: {
                    promotion_kind = Game::PromotionKind::Queen;
                    break;
                }
            }

            // Update the pending move with the chosen promotion kind
            Game::Move move(
                m_pending_promotion_move.from,
                m_pending_promotion_move.to,
                promotion_kind
            );

            // Complete the move
            auto move_made = m_game->MakeMove(move);
            _HandleMoveAftermath(move_made);

            m_promotion_dialog_active = false;
            current_promotion = 0;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_promotion_dialog_active = false;
            current_promotion = 0;
        }
    }

    ImGui::End();
}

ImVec2 ChessGUI::_GetScreenPos(Game::Coordinates coords) const
{
    if (m_is_normal_board_view)
    {

        auto x = m_board_startX + coords.GetFile() * m_square_size;
        auto y = m_board_startY + (7 - coords.GetRank()) * m_square_size;
        return ImVec2(x, y);
    }
    else
    {
        auto x = m_board_startX + coords.GetFile() * m_square_size;
        auto y = m_board_startY + coords.GetRank() * m_square_size;
        return ImVec2(x, y);
    }
}

std::optional<Game::Coordinates> ChessGUI::_GetCoordsFromScreenPos(ImVec2 pos) const
{
    // Calculate total board dimensions
    auto board_pixel_width = 8.0f * m_square_size;
    auto board_pixel_height = 8.0f * m_square_size;

    // Calculate potential board indices based on mouse position relative to board top-left
    auto relative_x = pos.x - m_board_startX;
    auto relative_y = pos.y - m_board_startY;

    // Check if the click is within the board's pixel boundaries
    if (relative_x < 0 || relative_x >= board_pixel_width || relative_y < 0 ||
        relative_y >= board_pixel_height)
    {
        return std::nullopt;
    }

    auto file = std::min(7, std::max(0, static_cast<int>(relative_x / m_square_size)));
    auto rank = std::min(
        7,
        std::max(
            0,
            m_is_normal_board_view ? 7 - static_cast<int>(relative_y / m_square_size)
                                   : static_cast<int>(relative_y / m_square_size)
        )
    );

    return Game::Coordinates(rank, file);
}

void ChessGUI::Render()
{
    if (!m_game)
    {
        Util::Debugger::Debug("Warning: ChessGUI::Render called with null game pointer\n");
        return;
    }

    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    auto window_size = viewport->WorkSize;

    // Non-interactive and borderless
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus |
                                    ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

    ImGui::Begin("Chess Game", nullptr, window_flags);

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Game"))
        {
            if (ImGui::MenuItem("Resign"))
            {
                m_game->Resign();
                m_draw_proposed = false;
                m_selected_square = std::nullopt;
                m_possible_moves_for_selected.clear();
            }

            if (m_draw_proposed)
            {
                if (m_draw_proposed_for == m_game->GetCurrentPlayer())
                {
                    if (ImGui::MenuItem("Accept Draw"))
                    {
                        m_game->Draw();
                        m_draw_proposed = false;
                        m_selected_square = std::nullopt;
                        m_possible_moves_for_selected.clear();
                    }
                    if (ImGui::MenuItem("Decline Draw"))
                    {
                        m_draw_proposed = false;
                    }
                }
            }
            else if (!m_draw_proposed && m_game->GetState() == Game::GameState::Waiting)
            {
                if (ImGui::MenuItem("Propose Draw"))
                {
                    m_draw_proposed = true;
                    m_draw_proposed_for = m_game->GetCurrentPlayer() == Game::Color::White
                                              ? Game::Color::Black
                                              : Game::Color::White;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Flip Board"))
            {
                m_is_normal_board_view = !m_is_normal_board_view;
            }

            if (ImGui::Checkbox("Flip Board on Move", &m_flip_board_on_move))
            {
                // no-op
            }

            ImGui::EndMenu();
        }

#ifndef NDEBUG
        if (ImGui::BeginMenu("Debug"))
        {
            bool debug_enabled = Util::Debugger::IsDebugEnabled();
            if (ImGui::Checkbox("Enable Debug Output", &debug_enabled))
            {
                Util::Debugger::SetDebugEnabled(debug_enabled);
            }

            ImGui::EndMenu();
        }
#endif
        ImGui::EndMainMenuBar();
    }

    auto turn = (m_game->GetCurrentPlayer() == Game::Color::White) ? "White" : "Black";

    auto state = m_game->GetState();
    switch (state)
    {
        case Game::GameState::Waiting: {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s to move", turn);
            break;
        }
        case Game::GameState::Ended: {
            auto opposite = (m_game->GetCurrentPlayer() == Game::Color::White) ? "Black" : "White";
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s won", opposite);
            break;
        }
        case Game::GameState::Draw: {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Draw by agreement");
            break;
        }
    }

    // Determine the largest possible square size that fits within the window
    m_square_size = std::min(window_size.x, window_size.y) / 8.0f;

    auto board_width = 8.0f * m_square_size;
    auto board_height = 8.0f * m_square_size;

    // Calculate the top-left starting position to center the board
    m_board_startX = viewport->WorkPos.x + (window_size.x - board_width) / 2.0f;
    m_board_startY = viewport->WorkPos.y + (window_size.y - board_height) / 2.0f;

    auto draw_list = ImGui::GetWindowDrawList();

    _DrawBoard(*draw_list);
    if (m_pieces_texture_id != 0)
    {
        _DrawPieces(*draw_list);
        // Draw highlights after pieces so they're visible on top
        _DrawHighlights(*draw_list);
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Piece textures not loaded!");
    }

    // Draw promotion dialog if active, otherwise handle input normally
    if (m_promotion_dialog_active)
    {
        _DrawPromotionDialog();
    }
    else
    {
        _HandleInput();
    }

    ImGui::End();
}

} // namespace GUI
