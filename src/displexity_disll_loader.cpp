#include "displexity_disll_loader.h"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>

namespace fs = std::filesystem;

#ifdef _WIN32
#include <windows.h>
static std::vector<HMODULE> g_handles;
#else
#include <dlfcn.h>
static std::vector<void*> g_handles;
#endif

void displexity_load_dislls_from_bindir(const char *bindir) {
    try {
        fs::path dir(bindir);
        if (!fs::exists(dir) || !fs::is_directory(dir)) return;

        for (auto &p : fs::directory_iterator(dir)) {
            if (!p.is_regular_file()) continue;
            std::string path = p.path().string();
            // Try to load the file as a dynamic library
#ifdef _WIN32
            HMODULE h = LoadLibraryA(path.c_str());
            if (h) {
                g_handles.push_back(h);
                std::cerr << "Loaded disll: " << path << "\n";
            } else {
                DWORD err = GetLastError();
                std::cerr << "Failed to load disll: " << path << " (err=" << err << ")\n";
            }
#else
            void *h = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
            if (h) {
                g_handles.push_back(h);
                std::cerr << "Loaded disll: " << path << "\n";
            } else {
                const char *err = dlerror();
                std::cerr << "Failed to load disll: " << path << " (" << (err?err:"unknown") << ")\n";
            }
#endif
        }
    } catch (const std::exception &e) {
        std::cerr << "disll loader error: " << e.what() << "\n";
    }
}
