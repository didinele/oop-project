// Neded for windows
#define NOMINMAX

#include "../Game/Piece/bishop_piece.h"
#include "../Game/Piece/king_piece.h"
#include "../Game/Piece/knight_piece.h"
#include "../Game/Piece/pawn_piece.h"
#include "../Game/Piece/piece.h"
#include "../Game/Piece/queen_piece.h"
#include "../Game/Piece/rook_piece.h"
#include "../Util/debug.h"
#include "chess_gui.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

namespace GUI
{
ChessGUI::ChessGUI(Game::Game *game) : m_game(game)
{
    if (!_LoadPiecesTexture("resources/textures/chess_pieces.png"))
    {
        std::cerr << "Failed to load chess pieces texture!" << std::endl;
    }
    Util::Debugger::Debug("ChessGUI is init. For further debug logs, use the Debug menu.\n");
    Util::Debugger::SetDebugEnabled(false);
}

ChessGUI::~ChessGUI()
{
    if (m_pieces_texture_id != 0)
    {
        glDeleteTextures(1, &m_pieces_texture_id);
        m_pieces_texture_id = 0;
    }
}

void ChessGUI::Render()
{
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

void ChessGUI::_HandleInput()
{
    auto scope = Util::Debugger::CreateScope("ChessGUI::HandleInput");

    static bool logged_game_over = false;
    if (m_game->GetState() != Game::GameState::Waiting)
    {
        if (!logged_game_over)
        {
            scope.Debug("Game is over. Ignoring mouse input.\n");
            logged_game_over = true;
        }
        return;
    }

    auto mouse_pos_rel = ImGui::GetMousePos();
    auto is_mouse_over_board = false;
    auto coords_option = _GetCoordsFromScreenPos(mouse_pos_rel);

    if (coords_option.has_value())
    {
        is_mouse_over_board = true;
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_mouse_over_board &&
        !m_promotion_dialog_active)
    {
        auto clicked_coords = coords_option.value();
        auto clicked_square_option = m_game->operator[](clicked_coords);

        if (m_selected_square.has_value())
        {
            bool is_move_target = false;
            Game::Move move_to_make(m_selected_square.value(), clicked_coords);

            for (const auto &possible_move : m_possible_moves_for_selected)
            {
                if (possible_move.to == clicked_coords)
                {
                    move_to_make = possible_move;
                    is_move_target = true;

                    if (possible_move.promotionKind.has_value())
                    {
                        // Don't make the move immediately, activate promotion dialog instead
                        m_promotion_dialog_active = true;
                        m_pending_promotion_move =
                            Game::Move(m_selected_square.value(), clicked_coords);

                        scope.Debug("Promotion move detected, showing dialog\n");
                        return;
                    }

                    break;
                }
            }

            if (is_move_target)
            {
                scope.Debug(
                    "Found within PossibleMoves, trying: from %d,%d to %d,%d.\n",
                    m_selected_square.value().GetRank(),
                    m_selected_square.value().GetFile(),
                    clicked_coords.GetRank(),
                    clicked_coords.GetFile()
                );

                auto move_made = m_game->MakeMove(move_to_make);
                scope.Debug("Move made: %s\n", move_made ? "true" : "false");

                _HandleMoveAftermath(move_made);
            }
            else
            {
                scope.Debug(
                    "Clicked square: %d,%d, but not a valid move target.\n",
                    clicked_coords.GetRank(),
                    clicked_coords.GetFile()
                );

                // If the user clicked a diff. piece of their color, select that instead
                if (clicked_square_option.has_value() &&
                    clicked_square_option.value()->GetColor() == m_game->GetCurrentPlayer())
                {
                    m_selected_square = clicked_coords;
                    auto board = m_game->GetBoard();
                    m_possible_moves_for_selected =
                        clicked_square_option.value()->GetPossibleMoves(board);
                }
                else
                {
                    // Clicked empty square or opponent piece - deselect
                    m_selected_square = std::nullopt;
                    m_possible_moves_for_selected.clear();
                }
            }
        }
        else
        {
            if (clicked_square_option.has_value() &&
                clicked_square_option.value()->GetColor() == m_game->GetCurrentPlayer())
            {
                // Clicked on a piece of the current player's color, select it
                m_selected_square = clicked_coords;
                // Get valid moves for the selected piece
                auto board = m_game->GetBoard();
                m_possible_moves_for_selected =
                    clicked_square_option.value()->GetPossibleMoves(board);
                scope.Debug(
                    "Selected square: %d,%d, possible move count: %zu\n",
                    clicked_coords.GetRank(),
                    clicked_coords.GetFile(),
                    m_possible_moves_for_selected.size()
                );
            }
            else
            {
                scope.Debug(
                    "Clicked square: %d,%d, but no piece or not the current player's "
                    "piece.\n",
                    clicked_coords.GetRank(),
                    clicked_coords.GetFile()
                );
                // Clicked empty square or opponent piece - do nothing
                m_selected_square = std::nullopt;
                m_possible_moves_for_selected.clear();
            }
        }
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !is_mouse_over_board)
    {
        // Clicked outside the board area, deselect anything selected
        m_selected_square = std::nullopt;
        m_possible_moves_for_selected.clear();
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

void ChessGUI::_HandleMoveAftermath(bool move_made)
{
    auto scope = Util::Debugger::CreateScope("ChessGUI::HandleMoveAftermath");

    if (move_made)
    {
        // Note the inverted check, since at this point GetCurrentPlayer is flipped
        if (m_draw_proposed && m_draw_proposed_for != m_game->GetCurrentPlayer())
        {
            // Implicitly rejected
            scope.Debug("Draw proposal implicitly rejected\n");
            m_draw_proposed = false;
        }

        if (m_flip_board_on_move)
        {
            m_is_normal_board_view = m_game->GetCurrentPlayer() == Game::Color::White;
            scope.Debug("Swapped board view\n");
        }
    }

    // Deselect regardless of outcome
    m_selected_square = std::nullopt;
    m_possible_moves_for_selected.clear();
}

bool ChessGUI::_LoadPiecesTexture(const char *filename)
{
    int width, height, channels_in_file;
    auto data = stbi_load(filename, &width, &height, &channels_in_file, 4);
    if (!data)
    {
        std::cerr << "Error loading texture: " << filename << " Reason: " << stbi_failure_reason()
                  << std::endl;
        return false;
    }

    m_texture_width = width;
    m_texture_height = height;

    glGenTextures(1, &m_pieces_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_pieces_texture_id);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload texture data
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        /* GL_RGBA */ 0x8058,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    // Free image data
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    Util::Debugger::Debug(
        "Loaded texture: %s (%d x %d, %d channels in file, loaded as RGBA)\n",
        filename,
        width,
        height,
        channels_in_file
    );

    return m_pieces_texture_id != 0;
}
} // namespace GUI
