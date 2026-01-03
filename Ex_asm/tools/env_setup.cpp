#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>

#ifdef _WIN32
static const char* NULL_REDIRECT = ">nul 2>nul";
#else
static const char* NULL_REDIRECT = ">/dev/null 2>/dev/null";
#endif

bool commandExists(const std::string &cmd) {
    std::string probe = "where " + cmd + " " + NULL_REDIRECT;
#ifdef __unix__
    probe = "which " + cmd + " " + NULL_REDIRECT;
#endif
    int rc = std::system(probe.c_str());
    return rc == 0;
}

int runCommand(const std::string &cmd) {
    std::cout << "[RUN] " << cmd << std::endl;
    int rc = std::system(cmd.c_str());
    std::cout << " => exit=" << rc << "\n";
    return rc;
}

int main(int argc, char** argv) {
    bool apply = false;
    bool dryRun = true;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--apply" || a == "-y") { apply = true; dryRun = false; }
        if (a == "--dry-run") { dryRun = true; }
    }

    std::cout << "Displexity Automated Environment Setup\n";
    std::cout << "Dry-run mode: " << (dryRun ? "YES" : "NO") << "\n";

    // Tools we need
    std::vector<std::string> tools = { "gcc", "g++", "nasm", "qemu-system-x86_64" };
    std::vector<std::string> missing;
    for (auto &t : tools) {
        if (!commandExists(t)) {
            missing.push_back(t);
            std::cout << "[MISSING] " << t << "\n";
        } else {
            std::cout << "[OK] " << t << "\n";
        }
    }

    if (missing.empty()) {
        std::cout << "All required tools detected. Nothing to do.\n";
        return 0;
    }

    // Detect package managers
    bool hasPacman = commandExists("pacman");
    bool hasChoco = commandExists("choco");
    bool hasScoop = commandExists("scoop");

    std::cout << "Detected package managers: pacman=" << hasPacman << ", choco=" << hasChoco << ", scoop=" << hasScoop << "\n";

    std::vector<std::string> cmds;

    if (hasPacman) {
        // MSYS2 / pacman path: include common dev libs (Vulkan, GLFW, GLM)
        std::ostringstream ss;
        ss << "pacman -S --needed --noconfirm ";
        ss << "mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-nasm ";
        ss << "mingw-w64-ucrt-x86_64-vulkan-devel mingw-w64-ucrt-x86_64-glfw ";
        ss << "mingw-w64-ucrt-x86_64-glm mingw-w64-ucrt-x86_64-qemu ";
        cmds.push_back(ss.str());
    } else if (hasChoco) {
        // Chocolatey
        cmds.push_back("choco install -y mingw nasm qemu-system-x86_64 glfw glm vulkan-sdk");
    } else if (hasScoop) {
        cmds.push_back("scoop install gcc nasm qemu glfw glm vulkan-sdk");
    } else {
        // Fallback: print instructions
        std::cout << "No supported package manager detected.\n";
        std::cout << "Please install MSYS2 (pacman) or Chocolatey and re-run with --apply.\n";
        std::cout << "Suggested commands:\n";
        std::cout << "  MSYS2 (run in MSYS2 shell): pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-nasm qemu\n";
        std::cout << "  Chocolatey (admin): choco install -y mingw nasm qemu-system-x86_64\n";
        return 2;
    }

    std::cout << "Commands to run:\n";
    for (auto &c : cmds) std::cout << "  " << c << "\n";

    if (dryRun) {
        std::cout << "Dry-run complete. Re-run with --apply or -y to execute.\n";
        return 0;
    }

    // Execute commands
    int overall = 0;
    for (auto &c : cmds) {
        int rc = runCommand(c);
        if (rc != 0) overall = rc;
    }

    if (overall == 0) std::cout << "Environment setup completed successfully.\n";
    else std::cout << "Environment setup finished with errors. See output above.\n";

    return overall;
}
