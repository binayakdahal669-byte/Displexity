#pragma once
#include <string>
#include <vector>

namespace ide {
    std::vector<std::string> get_outline_from_file(const std::string &path);
}
