#include "../Util/debug.h"
#include "chess_gui.h"
#include <GLFW/glfw3.h>
#include <cassert>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#undef STB_IMAGE_IMPLEMENTATION

namespace GUI
{
ChessGUI *ChessGUI::s_instance = nullptr;

ChessGUI &ChessGUI::GetInstance(Game::Game *game)
{
    assert(game != nullptr);
    if (!s_instance)
    {
        s_instance = new ChessGUI(game);
        Util::Debugger::Debug("ChessGUI singleton created.\n");
    }

    return *s_instance;
}

ChessGUI::ChessGUI(Game::Game *game) : m_game(game)
{
    if (!_LoadPiecesTexture("resources/textures/chess_pieces.png"))
    {
        std::cerr << "Failed to load chess pieces texture!" << std::endl;
    }

    Util::Debugger::Debug("ChessGUI is initialized. For further debug logs, use the Debug menu.\n");
    Util::Debugger::SetDebugEnabled(false);
}

void ChessGUI::DisposeInstance()
{
    delete s_instance;
    s_instance = nullptr;
}

ChessGUI::~ChessGUI()
{
    if (m_pieces_texture_id != 0)
    {
        glDeleteTextures(1, &m_pieces_texture_id);
        m_pieces_texture_id = 0;
    }
}

bool ChessGUI::_LoadPiecesTexture(const char *filename)
{
    int width, height, channels_in_file;
    unsigned char *data = stbi_load(filename, &width, &height, &channels_in_file, 4);
    if (data == nullptr)
    {
        std::cerr << "Failed to load texture: " << filename << std::endl
                  << "Reason: " << stbi_failure_reason() << std::endl;
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
