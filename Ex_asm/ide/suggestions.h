#pragma once
#include <string>
#include <vector>

namespace ide {
    std::vector<std::string> get_suggestions_from_file(const std::string &path, const std::string &prefix);
}
