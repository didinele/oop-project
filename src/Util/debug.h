#pragma once

#include <cstdarg>

namespace Util
{
class ScopedDebugger
{
  public:
    void Debug(const char *format, ...);

  private:
    const char *m_scope;

    ScopedDebugger(const char *scope);
    ScopedDebugger(const ScopedDebugger &) = delete;
    ScopedDebugger &operator=(const ScopedDebugger &) = delete;

    friend class Debugger;
};

class Debugger
{
  public:
    Debugger() = delete;
    Debugger(const Debugger &) = delete;
    Debugger &operator=(const Debugger &) = delete;

    static constexpr char s_debug_prefix[] = "[DEBUG] ";
    static constexpr auto s_debug_prefix_len = sizeof(s_debug_prefix) - 1;

    static void SetDebugEnabled(bool enabled);
    static void Debug(const char *format, ...);
    [[nodiscard]] static ScopedDebugger CreateScope(const char *scope);
#ifndef NDEBUG
    [[nodiscard]] static bool IsDebugEnabled();
#endif

  private:
#ifndef NDEBUG
    static bool s_debug_enabled;
#endif
    static void _Write(const char *format, std::va_list args);

    friend class ScopedDebugger;
};
} // namespace Util
