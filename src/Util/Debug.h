#pragma once

#include <cstdarg>

namespace util
{
class ScopedDebugger
{
  public:
    void Debug(const char *format, ...);

  private:
    const char *m_Scope;

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

    static constexpr char s_DebugPrefix[] = "[DEBUG] ";
    static constexpr auto s_DebugPrefixLen = sizeof(s_DebugPrefix) - 1;

    static void SetDebugEnabled(bool enabled);
    static void Debug(const char *format, ...);
    [[nodiscard]] static ScopedDebugger CreateScope(const char *scope);
#ifndef NDEBUG
    [[nodiscard]] static bool IsDebugEnabled();
#endif

  private:
#ifndef NDEBUG
    static bool s_DebugEnabled;
#endif
    static void Write(const char *format, std::va_list args);

    friend class ScopedDebugger;
};
} // namespace util
