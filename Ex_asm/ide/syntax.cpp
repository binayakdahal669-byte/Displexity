#include "syntax.h"
#include <regex>

namespace ide {

static const char* K_RESET = "\x1b[0m";
static const char* K_BOLD = "\x1b[1m";
// Colors tuned for dark/light (we pick color codes that work in common terminals)
static const char* K_KEYWORD = "\x1b[38;2;102;153;255m"; // blue-ish
static const char* K_TYPE = "\x1b[38;2;120;219;134m"; // green-ish
static const char* K_STRING = "\x1b[38;2;255;179;102m"; // orange-ish
static const char* K_COMMENT = "\x1b[38;2;128;128;128m"; // gray
static const char* K_NUMBER = "\x1b[38;2;255;153;153m"; // pink

std::string highlight_line(const std::string &line, bool /*darkTheme*/) {
    std::string out = line;
    // Strings first
    std::regex str_re("(\".*?\"|'.*?')");
    out = std::regex_replace(out, str_re, std::string(K_STRING) + "$1" + K_RESET);

    // Comments (//)
    std::regex comment_re("(//.*)$");
    out = std::regex_replace(out, comment_re, std::string(K_COMMENT) + "$1" + K_RESET);

    // Numbers
    std::regex num_re("\\b(\\d+)\\b");
    out = std::regex_replace(out, num_re, std::string(K_NUMBER) + "$1" + K_RESET);

    // Keywords
    std::vector<std::string> keywords = {"if","else","for","while","return","include","int","void","bool","class","struct","new","delete","using","namespace","static"};
    for (auto &kw : keywords) {
        std::regex kwr("\\b" + kw + "\\b");
        out = std::regex_replace(out, kwr, std::string(K_KEYWORD) + kw + K_RESET);
    }

    // Types (simple heuristic)
    std::vector<std::string> types = {"size_t","uint32_t","uint64_t","int32_t","float","double","char"};
    for (auto &t : types) {
        std::regex tr("\\b" + t + "\\b");
        out = std::regex_replace(out, tr, std::string(K_TYPE) + t + K_RESET);
    }

    return out;
}

} // namespace ide
