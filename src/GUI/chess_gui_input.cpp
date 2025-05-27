#include "../Util/debug.h"
#include "chess_gui.h"
#include <GLFW/glfw3.h>

namespace GUI
{

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
} // namespace GUI
