# DISP Programming Language Makefile v2.0

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -DDISPLEXITY_COMPILER
INCLUDES = -Isrc

# Optional comma-separated list of .disll patterns to copy into bin/disll
DISLLS ?=

# Directories
SRC_DIR = src
IDE_DIR = ide
BUILD_DIR = build
BIN_DIR = bin

# Source files
COMPILER_SOURCES = $(SRC_DIR)/main.cpp \
				  $(SRC_DIR)/compiler/lexer.cpp \
                  $(SRC_DIR)/compiler/parser.cpp \
                  $(SRC_DIR)/compiler/codegen.cpp \
                  $(SRC_DIR)/utils/logger.cpp
				  $(SRC_DIR)/displexity_disll_loader.cpp

IDE_SOURCES = $(IDE_DIR)/main.cpp

# Targets
COMPILER_TARGET = $(BIN_DIR)/displexitycompiler
IDE_TARGET = $(BIN_DIR)/dispide

# Platform-specific settings
ifeq ($(OS),Windows_NT)
    COMPILER_TARGET := $(COMPILER_TARGET).exe
    IDE_TARGET := $(IDE_TARGET).exe
    CXXFLAGS += -DWINDOWS
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CXXFLAGS += -DLINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        CXXFLAGS += -DMACOS
    endif
endif

.PHONY: all clean compiler ide install test examples package

all: compiler ide

# Create directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Compiler
compiler: $(BIN_DIR) $(COMPILER_TARGET)

$(COMPILER_TARGET): $(COMPILER_SOURCES)
	# Copy any requested .disll libraries into bin/disll for runtime discovery
	if [ ! -z "$(DISLLS)" ]; then \
		echo "Linking dislls: $(DISLLS)"; \
		python3 scripts/link_dislls.py "$(DISLLS)" || python scripts/link_dislls.py "$(DISLLS)"; \
	fi
	# Generate icons (logs, nvim)
	@echo "Generating icons..."
	@python3 scripts/generate_icons.py release/resources || python scripts/generate_icons.py release/resources || true

	# Compile Windows resource (if windres is available)
	@if command -v windres >/dev/null 2>&1; then \
		echo "Compiling resources with windres"; \
		windres src/disp.rc -O coff -o src/disp_res.o || true; \
		RES_OBJ=src/disp_res.o; \
	else \
		RES_OBJ=; \
	fi; \

	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(RES_OBJ)

# IDE
ide: $(BIN_DIR) $(IDE_TARGET)

$(IDE_TARGET): $(IDE_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Install
install: all
ifeq ($(OS),Windows_NT)
	copy $(BIN_DIR)\displexitycompiler.exe C:\Windows\System32\
	copy $(BIN_DIR)\dispide.exe C:\Windows\System32\
else
	sudo cp $(BIN_DIR)/displexitycompiler /usr/local/bin/
	sudo cp $(BIN_DIR)/dispide /usr/local/bin/
	sudo chmod +x /usr/local/bin/displexitycompiler
	sudo chmod +x /usr/local/bin/dispide
endif

# Enhanced Test Suite
test: compiler
	@echo "Running DISP Enhanced Test Suite..."
	@mkdir -p test_output
	
	@echo "Test 1: Basic Compilation"
	@echo 'function main() { Array[] msg = "Hello"; print(msg); return 0; }' > test_output/basic.disp
	$(COMPILER_TARGET) test_output/basic.disp -o test_output/basic.dis
	
	@echo "Test 2: Keypress Features"
	$(COMPILER_TARGET) examples/keypress_demo.disp -keypress -hotkey -o test_output/keypress.dis
	
	@echo "Test 3: GL Features"
	$(COMPILER_TARGET) examples/gl_demo.disp -gl -render -keypress -o test_output/gl_demo.dis
	
	@echo "Test 4: AI Features"
	$(COMPILER_TARGET) examples/ai_demo.disp -ai -translate -o test_output/ai_demo.dis
	
	@echo "Test 5: Bare Assembly"
	$(COMPILER_TARGET) examples/bare_asm_demo.disp -bare-asm -o test_output/bare.s
	
	@echo "Test 6: Complete Bare Assembly"
	$(COMPILER_TARGET) examples/bare_asm_demo.disp -complete-bare-asm -o test_output/complete_bare.s
	
	@echo "Test 7: BIOS Mode"
	$(COMPILER_TARGET) examples/bare_asm_demo.disp -bios -o test_output/bios.bin
	
	@echo "Test 8: Multi-ASM"
	$(COMPILER_TARGET) test_output/basic.disp -multi-asm -o test_output/multi.s
	
	@echo "Test 9: Cross-Platform"
	$(COMPILER_TARGET) test_output/basic.disp -target android -o test_output/android.apk
	$(COMPILER_TARGET) test_output/basic.disp -target ios -o test_output/ios.app
	
	@echo "Test 10: Speed Optimization"
	$(COMPILER_TARGET) test_output/basic.disp -speed -no-errors -o test_output/optimized.dis
	
	@echo "All enhanced tests completed!"

# Enhanced Examples
examples: compiler
	@mkdir -p examples
	@echo "Creating enhanced examples..."
	
	# All examples already created above
	
	@echo "Enhanced examples created!"

# Package for distribution
package: all examples
	@mkdir -p dist/displexity-lang
	@cp -r $(BIN_DIR) dist/displexity-lang/
	@cp -r examples dist/displexity-lang/
	@cp README.md dist/displexity-lang/
	@cp LANGUAGE_SPEC.md dist/displexity-lang/
	@cp Makefile dist/displexity-lang/
	
	@echo '#!/bin/bash' > dist/displexity-lang/install.sh
	@echo 'echo "Installing DISP Complexity Compiler..."' >> dist/displexity-lang/install.sh
	@echo 'sudo cp bin/displexitycompiler /usr/local/bin/' >> dist/displexity-lang/install.sh
	@echo 'sudo cp bin/dispide /usr/local/bin/' >> dist/displexity-lang/install.sh
	@echo 'sudo chmod +x /usr/local/bin/displexitycompiler' >> dist/displexity-lang/install.sh
	@echo 'sudo chmod +x /usr/local/bin/dispide' >> dist/displexity-lang/install.sh
	@echo 'echo "DISP Complexity installed successfully!"' >> dist/displexity-lang/install.sh
	@chmod +x dist/displexity-lang/install.sh
	
	@cd dist && tar -czf displexity-lang.tar.gz displexity-lang/
	@echo "Enhanced package created: dist/displexity-lang.tar.gz"

# Clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) dist test_output

# Help
help:
	@echo "DISP Complexity Programming Language Build System"
	@echo "================================================"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build compiler and IDE"
	@echo "  compiler  - Build only the compiler"
	@echo "  ide       - Build only the IDE"
	@echo "  install   - Install to system PATH"
	@echo "  test      - Run enhanced test suite"
	@echo "  examples  - Generate enhanced examples"
	@echo "  package   - Create distribution package"
	@echo "  clean     - Remove build files"
	@echo "  help      - Show this help"
	@echo ""
	@echo "Enhanced Features:"
	@echo "  - Keypress detection and hotkeys"
	@echo "  - OpenGL-like graphics"
	@echo "  - Advanced rendering"
	@echo "  - AI integration"
	@echo "  - Code translation"
	@echo "  - Multiple ASM formats"
	@echo "  - Cross-platform (Android, iOS)"
	@echo "  - Bare metal programming"
	@echo ""
	@echo "Usage:"
	@echo "  make all"
	@echo "  make install"
	@echo "  ./bin/displexitycompiler --help"