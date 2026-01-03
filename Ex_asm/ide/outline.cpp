#include "outline.h"
#include <fstream>
#include <sstream>
#include <regex>

namespace ide {

std::vector<std::string> get_outline_from_file(const std::string &path) {
    std::ifstream in(path);
    if (!in) return {};
    std::vector<std::string> outline;
    std::string line;
    std::regex func_re("^(\\s*(?:int|void|float|double|char|auto|bool|class|struct)\\s+([A-Za-z_][A-Za-z0-9_]*)\\s*\\(.*)$");
    std::regex class_re("^(\\s*(?:class|struct)\\s+([A-Za-z_][A-Za-z0-9_]*).*)");
    int lineno = 0;
    while (std::getline(in, line)) {
        lineno++;
        std::smatch m;
        if (std::regex_search(line, m, func_re)) {
            std::ostringstream o;
            o << lineno << ": " << m[2] << "()";
            outline.push_back(o.str());
        } else if (std::regex_search(line, m, class_re)) {
            std::ostringstream o;
            o << lineno << ": " << m[2] << " (type)";
            outline.push_back(o.str());
        }
    }
    return outline;
}

} // namespace ide
