#pragma once
#include <string>

namespace ide {
    // Return an ANSI-colored line for terminal display
    std::string highlight_line(const std::string &line, bool darkTheme = true);
}
