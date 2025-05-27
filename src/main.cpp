#include "GUI/gui_bootstrap.h"

int main(int, char **)
{
    auto window = GUI::GUIBootstrap::Bootstrap();
    if (!window) {
        return 1;
    }

    GUI::GUIBootstrap::Run(window);
    GUI::GUIBootstrap::Destroy(window);

    return 0;
}
