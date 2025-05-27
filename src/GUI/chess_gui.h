#pragma once

#include "../Game/game.h"
#include "../Game/move.h"
#include <imgui.h>
#include <imgui_impl_opengl3_loader.h>
#include <optional>
#include <vector>

namespace GUI
{

class ChessGUI
{
  public:
    ChessGUI(const ChessGUI&) = delete;
    ChessGUI& operator=(const ChessGUI&) = delete;

    [[nodiscard]] static ChessGUI& GetInstance(Game::Game* game);
    void DisposeInstance();

    void Render();

  private:
    ChessGUI(Game::Game* game);
    ~ChessGUI();

    static ChessGUI* s_instance;

    Game::Game* m_game;

    // Draws
    bool m_draw_proposed = false;
    Game::Color m_draw_proposed_for = Game::Color::White;

    // Board flipping
    bool m_flip_board_on_move = true;
    bool m_is_normal_board_view = true;

    // Promotion UI
    bool m_promotion_dialog_active = false;
    Game::Move m_pending_promotion_move =
        Game::Move(Game::Coordinates(0, 0), Game::Coordinates(0, 0));

    // UI stuff
    float m_square_size = 64.0f;
    float m_board_startX = 50.0f;
    float m_board_startY = 50.0f;
    // Light beige
    ImVec4 m_light_square_color = ImVec4(0.93f, 0.93f, 0.82f, 1.0f);
    // Green
    ImVec4 m_dark_square_color = ImVec4(0.48f, 0.62f, 0.44f, 1.0f);
    // Yellow highlight
    ImVec4 m_highlight_color = ImVec4(1.0f, 1.0f, 0.0f, 0.4f);

    // Game state interaction state
    std::optional<Game::Coordinates> m_selected_square = std::nullopt;
    std::vector<Game::Move> m_possible_moves_for_selected;

    // Texture related members
    GLuint m_pieces_texture_id = 0;
    int m_texture_width = 0;
    int m_texture_height = 0;

    // Drawing helpers
    void _DrawBoard(ImDrawList &draw_list);
    void _DrawPieces(ImDrawList &draw_list);
    void _DrawHighlights(ImDrawList &draw_list);
    void _DrawPromotionDialog();

    // Input handling
    void _HandleInput();

    // Common move processing
    void _HandleMoveAftermath(bool move_made);

    // Coordinate conversion helpers
    ImVec2 _GetScreenPos(Game::Coordinates coords) const;
    std::optional<Game::Coordinates> _GetCoordsFromScreenPos(ImVec2 pos) const;

    // Texture loading helper
    bool _LoadPiecesTexture(const char *filename);
};
} // namespace GUI
