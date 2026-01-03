# Displexity IDE (Vulkan+GLFW) - Prototype

This is a minimal scaffold for a modern IDE using Vulkan + GLFW + GLM.

Requirements (MSYS2 UCRT64):

- mingw-w64-ucrt-x86_64-glfw
- mingw-w64-ucrt-x86_64-vulkan-devel
- mingw-w64-ucrt-x86_64-glm
- mingw-w64-ucrt-x86_64-gcc

Install on MSYS2 UCRT64 shell:

```bash
pacman -S --needed --noconfirm mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-vulkan-devel mingw-w64-ucrt-x86_64-glm mingw-w64-ucrt-x86_64-gcc
```

Build (MSYS2 UCRT64):

```bash
g++ -O2 -std=c++17 -municode -mwindows -I/mingw64/include -L/mingw64/lib ide/main.cpp -o ide/displexity-ide.exe -lglfw -lvulkan
```

Notes:
- This prototype only creates a GLFW window and queries Vulkan instance extensions.
- Next steps: integrate Dear ImGui (ImGui + backends), add text editor widget (e.g., Eastl or custom), implement project tree, compiler integration and build panel.
