#pragma once

#include <stdarg.h>  // For va_start, etc.

#include <string>

extern std::string string_format(const std::string fmt_str, ...);