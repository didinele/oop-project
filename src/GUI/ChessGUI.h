#pragma once

#include "../Game/Game.h"
#include "../Game/Move.h"
#include <imgui.h>
#include <imgui_impl_opengl3_loader.h>
#include <optional>
#include <vector>

namespace gui
{

class ChessGUI
{
  public:
    ChessGUI(game::Game *game);
    ~ChessGUI();

    void Render();

  private:
    game::Game *m_Game;
    
    bool m_SwapBoardViewOnMove = true;
    bool m_IsNormalBoardView = true;

    float m_SquareSize = 64.0f;
    float m_BoardStartX = 50.0f;
    float m_BoardStartY = 50.0f;
    // Light beige
    ImVec4 m_LightSquareColor = ImVec4(0.93f, 0.93f, 0.82f, 1.0f);
    // Green
    ImVec4 m_DarkSquareColor = ImVec4(0.48f, 0.62f, 0.44f, 1.0f);
    // Yellow highlight
    ImVec4 m_HighlightColor = ImVec4(1.0f, 1.0f, 0.0f, 0.4f);

    // Game state interaction
    std::optional<game::Coordinates> m_SelectedSquare = std::nullopt;
    std::vector<game::Move> m_PossibleMovesForSelected;

    // Texture related members
    GLuint m_PiecesTextureID = 0;
    int m_TextureWidth = 0;
    int m_TextureHeight = 0;

    // Drawing helpers
    void DrawBoard(ImDrawList &draw_list);
    void DrawPieces(ImDrawList &draw_list);

    // Input handling
    void HandleInput();

    // Coordinate conversion helpers
    ImVec2 GetScreenPos(game::Coordinates coords) const;
    std::optional<game::Coordinates> GetCoordsFromScreenPos(ImVec2 pos) const;

    // Texture loading helper
    bool LoadPiecesTexture(const char *filename);
};
} // namespace gui
