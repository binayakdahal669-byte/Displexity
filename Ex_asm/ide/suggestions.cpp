#include "suggestions.h"
#include <fstream>
#include <sstream>
#include <set>
#include <regex>

namespace ide {

static std::vector<std::string> tokenize_words(const std::string &s) {
    std::vector<std::string> out;
    std::regex re("[A-Za-z_][A-Za-z0-9_]*");
    auto begin = std::sregex_iterator(s.begin(), s.end(), re);
    auto end = std::sregex_iterator();
    for (auto it = begin; it != end; ++it) out.push_back(it->str());
    return out;
}

std::vector<std::string> get_suggestions_from_file(const std::string &path, const std::string &prefix) {
    std::ifstream in(path);
    if (!in) return {};
    std::ostringstream ss;
    ss << in.rdbuf();
    std::string content = ss.str();
    auto words = tokenize_words(content);
    std::set<std::string> unique;
    for (auto &w : words) {
        if (w.size() >= prefix.size() && w.substr(0, prefix.size()) == prefix) unique.insert(w);
    }
    return std::vector<std::string>(unique.begin(), unique.end());
}

} // namespace ide
