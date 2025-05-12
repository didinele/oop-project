// TODO: Highlights should be on top of pieces

// No idea how this header is written, but if its not our first include, we aren't compiling
#include <OpenGL/gl3.h>

#include "../Game/Piece/BishopPiece.h"
#include "../Game/Piece/KingPiece.h"
#include "../Game/Piece/KnightPiece.h"
#include "../Game/Piece/PawnPiece.h"
#include "../Game/Piece/Piece.h"
#include "../Game/Piece/QueenPiece.h"
#include "../Game/Piece/RookPiece.h"
#include "../Util/Debug.h"
#include "ChessGUI.h"
#include "imgui.h"
#include <algorithm>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

namespace gui
{
bool ChessGUI::LoadPiecesTexture(const char *filename)
{
    int width, height, channels_in_file;
    auto data = stbi_load(filename, &width, &height, &channels_in_file, 4);
    if (!data)
    {
        std::cerr << "Error loading texture: " << filename << " Reason: " << stbi_failure_reason()
                  << std::endl;
        return false;
    }

    m_TextureWidth = width;
    m_TextureHeight = height;

    glGenTextures(1, &m_PiecesTextureID);
    glBindTexture(GL_TEXTURE_2D, m_PiecesTextureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Free image data
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    util::Debugger::Debug(
        "Loaded texture: %s (%d x %d, %d channels in file, loaded as RGBA)\n",
        filename,
        width,
        height,
        channels_in_file
    );

    return m_PiecesTextureID != 0;
}

ChessGUI::ChessGUI(game::Game *game) : m_Game(game)
{
    if (!LoadPiecesTexture("resources/textures/chess_pieces.png"))
    {
        std::cerr << "Failed to load chess pieces texture!" << std::endl;
    }
    util::Debugger::Debug("ChessGUI is init. For further debug logs, use the Debug menu.\n");
    util::Debugger::SetDebugEnabled(false);
}

ChessGUI::~ChessGUI()
{
    if (m_PiecesTextureID != 0)
    {
        glDeleteTextures(1, &m_PiecesTextureID);
        m_PiecesTextureID = 0;
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
                m_Game->Resign();
                m_DrawProposed = false;
                m_SelectedSquare = std::nullopt;
                m_PossibleMovesForSelected.clear();
            }

            if (m_DrawProposed)
            {
                if (m_DrawProposedFor == m_Game->GetCurrentPlayer())
                {

                    if (ImGui::MenuItem("Accept Draw"))
                    {
                        m_Game->Draw();
                        m_DrawProposed = false;
                        m_SelectedSquare = std::nullopt;
                        m_PossibleMovesForSelected.clear();
                    }
                    if (ImGui::MenuItem("Decline Draw"))
                    {
                        m_DrawProposed = false;
                    }
                }
            }
            else if (!m_DrawProposed && m_Game->GetState() == game::GameState::Waiting)
            {
                if (ImGui::MenuItem("Propose Draw"))
                {
                    m_DrawProposed = true;
                    m_DrawProposedFor = m_Game->GetCurrentPlayer() == game::Color::White
                                            ? game::Color::Black
                                            : game::Color::White;
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Flip Board"))
            {
                m_IsNormalBoardView = !m_IsNormalBoardView;
            }

            if (ImGui::Checkbox("Flip Board on Move", &m_FlipBoardOnMove))
            {
                // no-op
            }

            ImGui::EndMenu();
        }

#ifndef NDEBUG
        if (ImGui::BeginMenu("Debug"))
        {
            bool debug_enabled = util::Debugger::IsDebugEnabled();
            if (ImGui::Checkbox("Enable Debug Output", &debug_enabled))
            {
                util::Debugger::SetDebugEnabled(debug_enabled);
            }

            ImGui::EndMenu();
        }
#endif
        ImGui::EndMainMenuBar();
    }

    auto turn = (m_Game->GetCurrentPlayer() == game::Color::White) ? "White" : "Black";

    auto state = m_Game->GetState();
    switch (state)
    {
        case game::GameState::Waiting: {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s to move", turn);
            break;
        }
        case game::GameState::Ended: {
            auto opposite = (m_Game->GetCurrentPlayer() == game::Color::White) ? "Black" : "White";
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s won", opposite);
            break;
        }
        case game::GameState::Draw: {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Draw by agreement");
            break;
        }
    }

    // Determine the largest possible square size that fits within the window
    m_SquareSize = std::min(window_size.x, window_size.y) / 8.0f;

    auto board_width = 8.0f * m_SquareSize;
    auto board_height = 8.0f * m_SquareSize;

    // Calculate the top-left starting position to center the board
    m_BoardStartX = viewport->WorkPos.x + (window_size.x - board_width) / 2.0f;
    m_BoardStartY = viewport->WorkPos.y + (window_size.y - board_height) / 2.0f;

    auto draw_list = ImGui::GetWindowDrawList();

    DrawBoard(*draw_list);
    if (m_PiecesTextureID != 0)
    {
        DrawPieces(*draw_list);
    }
    else
    {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error: Piece textures not loaded!");
    }

    // Draw promotion dialog if active, otherwise handle input normally
    if (m_PromotionDialogActive)
    {
        DrawPromotionDialog();
    }
    else
    {
        HandleInput();
    }

    ImGui::End();
}

void ChessGUI::DrawBoard(ImDrawList &draw_list)
{
    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            auto in_light_square = (rank + file) % 2 != 0;
            ImVec4 square_color = in_light_square ? m_LightSquareColor : m_DarkSquareColor;

            game::Coordinates coords(rank, file);
            ImVec2 p_min = GetScreenPos(coords);
            ImVec2 p_max = ImVec2(p_min.x + m_SquareSize, p_min.y + m_SquareSize);

            draw_list.AddRectFilled(p_min, p_max, ImGui::ColorConvertFloat4ToU32(square_color));

            if (m_SelectedSquare.has_value() && m_SelectedSquare.value() == coords)
            {
                draw_list
                    .AddRectFilled(p_min, p_max, ImGui::ColorConvertFloat4ToU32(m_HighlightColor));
            }
        }
    }

    if (m_SelectedSquare.has_value())
    {
        for (auto &move : m_PossibleMovesForSelected)
        {
            ImVec2 p_min = GetScreenPos(move.to);
            // Draw a circle or different highlight for possible moves
            draw_list.AddCircleFilled(
                ImVec2(p_min.x + m_SquareSize * 0.5f, p_min.y + m_SquareSize * 0.5f),
                m_SquareSize * 0.2f, // Smaller radius for move indicators
                ImGui::ColorConvertFloat4ToU32(m_HighlightColor)
            );
        }
    }
}

void ChessGUI::DrawPieces(ImDrawList &drawList)
{
    if (m_PiecesTextureID == 0 || m_TextureWidth == 0 || m_TextureHeight == 0)
    {
        return;
    }

    // Width of one piece in texture
    const auto piece_tex_width = static_cast<float>(m_TextureWidth) / 6.0f;
    // Height of one piece in texture
    const auto piece_tex_height = static_cast<float>(m_TextureHeight) / 2.0f;

    for (int rank = 0; rank < 8; ++rank)
    {
        for (int file = 0; file < 8; ++file)
        {
            auto coords = game::Coordinates(rank, file);
            if (m_Game->operator[](coords).has_value())
            {
                auto piece = m_Game->operator[](coords).value();

                // Column index in the texture (0-5)
                int col = -1;
                // Row index in the texture atlas (0=white, 1=black)
                int row = -1;

                if (dynamic_cast<const game::KingPiece *>(piece))
                    col = 0;
                else if (dynamic_cast<const game::QueenPiece *>(piece))
                    col = 1;
                else if (dynamic_cast<const game::BishopPiece *>(piece))
                    col = 2;
                else if (dynamic_cast<const game::KnightPiece *>(piece))
                    col = 3;
                else if (dynamic_cast<const game::RookPiece *>(piece))
                    col = 4;
                else if (dynamic_cast<const game::PawnPiece *>(piece))
                    col = 5;

                if (piece->GetColor() == game::Color::White)
                    row = 0;
                else if (piece->GetColor() == game::Color::Black)
                    row = 1;

                if (col != -1 && row != -1)
                {
                    // Calculate coordinates
                    auto u0 = (col * piece_tex_width) / m_TextureWidth;
                    auto v0 = (row * piece_tex_height) / m_TextureHeight;
                    auto u1 = ((col + 1) * piece_tex_width) / m_TextureWidth;
                    auto v1 = ((row + 1) * piece_tex_height) / m_TextureHeight;

                    // Get screen position for the square
                    ImVec2 p_min = GetScreenPos(coords);
                    ImVec2 p_max = ImVec2(p_min.x + m_SquareSize, p_min.y + m_SquareSize);

                    drawList
                        .AddImage(m_PiecesTextureID, p_min, p_max, ImVec2(u0, v0), ImVec2(u1, v1));
                }
            }
        }
    }
}

void ChessGUI::HandleInput()
{
    auto scope = util::Debugger::CreateScope("ChessGUI::HandleInput");

    static bool logged_game_over = false;
    if (m_Game->GetState() != game::GameState::Waiting)
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
    auto coords_option = GetCoordsFromScreenPos(mouse_pos_rel);

    if (coords_option.has_value())
    {
        is_mouse_over_board = true;
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && is_mouse_over_board &&
        !m_PromotionDialogActive)
    {
        auto clicked_coords = coords_option.value();
        auto clicked_square_option = m_Game->operator[](clicked_coords);

        if (m_SelectedSquare.has_value())
        {
            bool is_move_target = false;
            game::Move move_to_make(m_SelectedSquare.value(), clicked_coords);

            for (const auto &possible_move : m_PossibleMovesForSelected)
            {
                if (possible_move.to == clicked_coords)
                {
                    move_to_make = possible_move;
                    is_move_target = true;

                    if (possible_move.promotionKind.has_value())
                    {
                        // Don't make the move immediately, activate promotion dialog instead
                        m_PromotionDialogActive = true;
                        m_PendingPromotionMove =
                            game::Move(m_SelectedSquare.value(), clicked_coords);

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
                    m_SelectedSquare.value().GetRank(),
                    m_SelectedSquare.value().GetFile(),
                    clicked_coords.GetRank(),
                    clicked_coords.GetFile()
                );

                auto move_made = m_Game->MakeMove(move_to_make);
                scope.Debug("Move made: %s\n", move_made ? "true" : "false");

                if (move_made)
                {
                    // Note the inverted check, since at this point GetCurrentPlayer is flipped
                    if (m_DrawProposed && m_DrawProposedFor != m_Game->GetCurrentPlayer())
                    {
                        // Implicitly rejected
                        scope.Debug("Draw proposal implicitly rejected\n");
                        m_DrawProposed = false;
                    }

                    if (m_FlipBoardOnMove)
                    {
                        m_IsNormalBoardView = m_Game->GetCurrentPlayer() == game::Color::White;
                        scope.Debug("Swapped board view\n");
                    }
                }

                // Deselect regardless of outcome
                m_SelectedSquare = std::nullopt;
                m_PossibleMovesForSelected.clear();
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
                    clicked_square_option.value()->GetColor() == m_Game->GetCurrentPlayer())
                {
                    m_SelectedSquare = clicked_coords;
                    auto board = m_Game->GetBoard();
                    m_PossibleMovesForSelected =
                        clicked_square_option.value()->GetPossibleMoves(board);
                }
                else
                {
                    // Clicked empty square or opponent piece - deselect
                    m_SelectedSquare = std::nullopt;
                    m_PossibleMovesForSelected.clear();
                }
            }
        }
        else
        {
            if (clicked_square_option.has_value() &&
                clicked_square_option.value()->GetColor() == m_Game->GetCurrentPlayer())
            {
                // Clicked on a piece of the current player's color, select it
                m_SelectedSquare = clicked_coords;
                // Get valid moves for the selected piece
                auto board = m_Game->GetBoard();
                m_PossibleMovesForSelected = clicked_square_option.value()->GetPossibleMoves(board);
                scope.Debug(
                    "Selected square: %d,%d, possible move count: %zu\n",
                    clicked_coords.GetRank(),
                    clicked_coords.GetFile(),
                    m_PossibleMovesForSelected.size()
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
                m_SelectedSquare = std::nullopt;
                m_PossibleMovesForSelected.clear();
            }
        }
    }
    else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !is_mouse_over_board)
    {
        // Clicked outside the board area, deselect anything selected
        m_SelectedSquare = std::nullopt;
        m_PossibleMovesForSelected.clear();
    }
}

void ChessGUI::DrawPromotionDialog()
{
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

    if (ImGui::Begin("Promote Pawn", &m_PromotionDialogActive, flags))
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
            game::PromotionKind promotion_kind;

            switch (current_promotion)
            {
                case 0: {
                    promotion_kind = game::PromotionKind::Queen;
                    break;
                }
                case 1: {
                    promotion_kind = game::PromotionKind::Rook;
                    break;
                }
                case 2: {
                    promotion_kind = game::PromotionKind::Bishop;
                    break;
                }
                case 3: {
                    promotion_kind = game::PromotionKind::Knight;
                    break;
                }
                default: {
                    promotion_kind = game::PromotionKind::Queen;
                    break;
                }
            }

            // Update the pending move with the chosen promotion kind
            game::Move move(m_PendingPromotionMove.from, m_PendingPromotionMove.to, promotion_kind);

            // TODO: This stuff should be de-duped with handle input

            // Complete the move
            auto move_made = m_Game->MakeMove(move);
            if (move_made)
            {
                // Note the inverted check, since at this point GetCurrentPlayer is flipped
                if (m_DrawProposed && m_DrawProposedFor != m_Game->GetCurrentPlayer())
                {
                    // Implicitly rejected
                    m_DrawProposed = false;
                }

                if (m_FlipBoardOnMove)
                {
                    m_IsNormalBoardView = m_Game->GetCurrentPlayer() == game::Color::White;
                }
            }

            // Close dialog and reset selection
            m_PromotionDialogActive = false;
            current_promotion = 0;

            // Clear selection
            m_SelectedSquare = std::nullopt;
            m_PossibleMovesForSelected.clear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            m_PromotionDialogActive = false;
            current_promotion = 0;
        }
    }

    ImGui::End();
}

ImVec2 ChessGUI::GetScreenPos(game::Coordinates coords) const
{
    if (m_IsNormalBoardView)
    {

        auto x = m_BoardStartX + coords.GetFile() * m_SquareSize;
        auto y = m_BoardStartY + (7 - coords.GetRank()) * m_SquareSize;
        return ImVec2(x, y);
    }
    else
    {
        auto x = m_BoardStartX + coords.GetFile() * m_SquareSize;
        auto y = m_BoardStartY + coords.GetRank() * m_SquareSize;
        return ImVec2(x, y);
    }
}

std::optional<game::Coordinates> ChessGUI::GetCoordsFromScreenPos(ImVec2 pos) const
{
    // Calculate total board dimensions
    auto board_pixel_width = 8.0f * m_SquareSize;
    auto board_pixel_height = 8.0f * m_SquareSize;

    // Calculate potential board indices based on mouse position relative to board top-left
    auto relative_x = pos.x - m_BoardStartX;
    auto relative_y = pos.y - m_BoardStartY;

    // Check if the click is within the board's pixel boundaries
    if (relative_x < 0 || relative_x >= board_pixel_width || relative_y < 0 ||
        relative_y >= board_pixel_height)
    {
        return std::nullopt;
    }

    auto file = std::min(7, std::max(0, static_cast<int>(relative_x / m_SquareSize)));
    auto rank = std::min(
        7,
        std::max(
            0,
            m_IsNormalBoardView ? 7 - static_cast<int>(relative_y / m_SquareSize)
                                : static_cast<int>(relative_y / m_SquareSize)
        )
    );

    return game::Coordinates(rank, file);
}
} // namespace gui
