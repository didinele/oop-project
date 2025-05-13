#include "debug.h"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

namespace util
{
ScopedDebugger::ScopedDebugger(const char *scope) : m_Scope(scope)
{
}

void ScopedDebugger::Debug(const char *format, ...)
{
#ifndef NDEBUG
    if (!Debugger::s_DebugEnabled)
    {
        return;
    }

    // Add the scope to the debug format as [Scope]
    auto scope_len = std::strlen(m_Scope);
    auto format_len = std::strlen(format);
    // 4 for [], a space, and null terminator
    auto buffer_size = scope_len + format_len + 4;
    std::vector<char> buffer(buffer_size);

    auto written = std::snprintf(buffer.data(), buffer_size, "[%s] %s", m_Scope, format);
    assert(written > 0 && static_cast<unsigned long>(written) < buffer_size);

    va_list args;
    va_start(args, format);
    Debugger::Write(buffer.data(), args);
    va_end(args);
#else
    return;
#endif
}

#ifndef NDEBUG
bool Debugger::s_DebugEnabled = true;
#endif

void Debugger::SetDebugEnabled(bool enabled)
{
#ifndef NDEBUG
    s_DebugEnabled = enabled;
#endif
}

void Debugger::Debug(const char *format, ...)
{
#ifndef NDEBUG
    if (!s_DebugEnabled)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    Write(format, args);
    va_end(args);
#else
    return;
#endif
}

ScopedDebugger Debugger::CreateScope(const char *scope)
{
    return ScopedDebugger(scope);
}

void Debugger::Write(const char *format, va_list args)
{
    auto required_size_without_prefix = std::vsnprintf(NULL, 0, format, args);

    auto buffer_size = s_DebugPrefixLen + required_size_without_prefix + 1;
    std::vector<char> buffer(buffer_size);
    std::strncpy(buffer.data(), s_DebugPrefix, s_DebugPrefixLen);
    auto written = std::vsnprintf(
        buffer.data() + s_DebugPrefixLen,
        buffer_size - s_DebugPrefixLen,
        format,
        args
    );
    assert(written > 0 && static_cast<unsigned long>(written) < buffer_size);
    std::printf("%s", buffer.data());
}

#ifndef NDEBUG
bool Debugger::IsDebugEnabled()
{
    return s_DebugEnabled;
}
#endif
} // namespace util
