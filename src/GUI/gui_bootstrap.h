#pragma once

#include <GLFW/glfw3.h>

namespace GUI
{
class GUIBootstrap
{
  public:
    static GLFWwindow *Bootstrap();

    static void Run(GLFWwindow *window);
    static void Destroy(GLFWwindow *window);

  private:
    GUIBootstrap() = delete;

    static void _ErrorCallback(int error, const char *description);
};

} // namespace GUI
