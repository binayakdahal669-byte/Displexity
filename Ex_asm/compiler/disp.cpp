#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "asmgen.h"
#include "biosgen.h"
#include "armgen.h"
#include "riscvgen.h"
#include <ctime>
#include <iomanip>
#include <filesystem>

using namespace std;
using namespace disp;

string basename(const string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos != string::npos) return path.substr(pos + 1);
    return path;
}

string removeExtension(const string& name) {
    size_t pos = name.find_last_of('.');
    if (pos != string::npos) return name.substr(0, pos);
    return name;
}

// .disp package format: [MAGIC:8][VERSION:4][BINARY_SIZE:8][BINARY_DATA...]
struct DispPackage {
    static const string MAGIC;
    
    static void create(const string &exePath, const string &dispPath) {
        ifstream exe(exePath, ios::binary);
        if (!exe) throw runtime_error("Cannot read exe: " + exePath);
        
        stringstream ss;
        ss << exe.rdbuf();
        string binaryData = ss.str();
        exe.close();
        
        ofstream disp(dispPath, ios::binary);
        if (!disp) throw runtime_error("Cannot write disp: " + dispPath);
        
        // Write header
        disp << MAGIC;  // "DISPLX01" (8 bytes)
        uint32_t version = 1;
        disp.write((char*)&version, 4);
        uint64_t binSize = binaryData.size();
        disp.write((char*)&binSize, 8);
        // Write binary
        disp.write(binaryData.c_str(), binaryData.size());
        disp.close();
    }
    
    static void extract(const string &dispPath, const string &exePath) {
        ifstream disp(dispPath, ios::binary);
        if (!disp) throw runtime_error("Cannot read disp: " + dispPath);
        
        char magic[9] = {0};
        disp.read(magic, 8);
        if (string(magic) != MAGIC) throw runtime_error("Invalid .disp file");
        
        uint32_t version;
        disp.read((char*)&version, 4);
        uint64_t binSize;
        disp.read((char*)&binSize, 8);
        
        vector<char> binary(binSize);
        disp.read(binary.data(), binSize);
        disp.close();
        
        ofstream exe(exePath, ios::binary);
        if (!exe) throw runtime_error("Cannot write exe: " + exePath);
        exe.write(binary.data(), binSize);
        exe.close();
    }
};
const string DispPackage::MAGIC = "DISPLX01";

// Config file management (.disprc)
class DispConfig {
public:
    map<string, string> settings;
    string configPath;
    
    DispConfig() {
        #ifdef _WIN32
            const char *home = getenv("USERPROFILE");
        #else
            const char *home = getenv("HOME");
        #endif
        configPath = string(home ? home : ".") + "/.disprc";
        load();
    }
    
    void load() {
        ifstream cfg(configPath);
        if (!cfg) return;
        string line;
        while (getline(cfg, line)) {
            size_t eq = line.find('=');
            if (eq != string::npos) {
                string key = line.substr(0, eq);
                string val = line.substr(eq + 1);
                settings[key] = val;
            }
        }
        cfg.close();
    }
    
    void save() {
        ofstream cfg(configPath);
        for (auto &kv : settings) {
            cfg << kv.first << "=" << kv.second << "\n";
        }
        cfg.close();
    }
    
    string get(const string &key, const string &defaultVal = "") {
        auto it = settings.find(key);
        return it != settings.end() ? it->second : defaultVal;
    }
    
    void set(const string &key, const string &val) {
        settings[key] = val;
        save();
    }
};

int handleActivate() {
    cout << "Activating Displexity compiler...\n";
    #ifdef _WIN32
        // Add to PATH via registry or print instructions
        cout << "To add disp to PATH on Windows:\n";
        cout << "  1. setx PATH \"%PATH%;E:\\Ex_asm\\build\"\n";
        cout << "  2. Restart terminal\n";
        cout << "Or copy disp.exe to a folder already in PATH (e.g., C:\\Windows\\System32)\n";
    #else
        cout << "To add disp to PATH on Linux/macOS:\n";
        cout << "  1. export PATH=\"$PATH:/path/to/Ex_asm/build\"\n";
        cout << "  2. Add to ~/.bashrc or ~/.zshrc to make it permanent\n";
        cout << "Or: sudo ln -s /path/to/Ex_asm/build/disp /usr/local/bin/disp\n";
    #endif
    
    DispConfig cfg;
    cfg.set("auto_startup", "true");
    cout << "Config saved to: " << cfg.configPath << "\n";
    return 0;
}

int handleConfig(int argc, char **argv) {
    DispConfig cfg;
    
    if (argc < 4) {
        cout << "Usage: disp config <key> [value]\n";
        cout << "Config file: " << cfg.configPath << "\n";
        cout << "Settings:\n";
        for (auto &kv : cfg.settings) {
            cout << "  " << kv.first << " = " << kv.second << "\n";
        }
        return 0;
    }
    
    string key = argv[2];
    if (argc > 3) {
        string val = argv[3];
        cfg.set(key, val);
        cout << "Set " << key << " = " << val << "\n";
    } else {
        cout << key << " = " << cfg.get(key, "(not set)") << "\n";
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "disp - Displexity compiler\n";
        cout << "Usage: disp <input.dis> [options]\n";
        cout << "Commands: disp activate, disp config, disp --help\n";
        return 1;
    }

    // Handle special commands
    string firstArg = argv[1];
    if (firstArg == "activate") {
        return handleActivate();
    }
    if (firstArg == "config") {
        return handleConfig(argc, argv);
    }
    if (firstArg == "run" && argc > 2) {
        // disp run file.disp - extract and run
        try {
            string dispFile = argv[2];
            string tmpExe = removeExtension(dispFile) + "_tmp_run.exe";
            DispPackage::extract(dispFile, tmpExe);
            int ret = system(tmpExe.c_str());
            namespace fs = std::filesystem;
            try { fs::remove(tmpExe); } catch (...) {}
            return ret >> 8;
        } catch (const exception &e) {
            cerr << "Error: " << e.what() << "\n";
            return 1;
        }
    }
    
    string inputFile;
    string outputFile;
    string emitMode = "c";  // c, asm, exe, disp, bare-asm
    bool verbose = false;
    string logFileOverride;

    auto printHelp = [&]() {
        cout << "disp - Displexity Language Compiler\n\n";
        cout << "Usage: disp <input.dis> [options]\n\n";
        cout << "Compile Commands:\n";
        cout << "  -o <file>              Output file (default: input.exe)\n";
        cout << "  -emit-c                Emit C source only (do not compile)\n";
        cout << "  -emit-exe              Emit native executable (auto-compiles via GCC)\n";
        cout << "  -emit-asm              Emit x86-64 assembly (not yet fully implemented)\n";
        cout << "  -emit-disp             Emit .disp package (bundled executable)\n";
        cout << "  -bare-asm              Emit bare-metal x86 assembly for BIOS/boot\n";
        cout << "  -emit-arm              Emit ARM64 (aarch64) assembly\n";
        cout << "  -emit-riscv            Emit RISC-V RV64I assembly\n";
        cout << "  --log <file>           Write compilation log to given file\n";
        cout << "  -v, --verbose          Increase output verbosity\n";
        cout << "\nSpecial Commands:\n";
        cout << "  disp activate          Add disp to PATH and enable auto-startup\n";
        cout << "  disp config [key val]  Get/set configuration (e.g., search paths)\n";
        cout << "  disp run file.disp     Extract and execute a .disp package\n";
        cout << "  disp --help            Show this help\n";
        cout << "  disp --version         Show version\n";
        cout << "\nFile Extensions:\n";
        cout << "  .dis                   Displexity source file\n";
        cout << "  .dish                  Displexity header file\n";
        cout << "  .disll                 Displexity library file\n";
        cout << "  .disp                  Displexity executable/package (binary format)\n";
        cout << "\nExamples:\n";
        cout << "  disp hello.dis                         -> hello.exe\n";
        cout << "  disp hello.dis -o hello.disp           -> hello.disp (packaged)\n";
        cout << "  disp hello.dis -emit-c -o hello.c      -> hello.c (C only)\n";
        cout << "  disp run hello.disp                    -> extract and run\n";
        cout << "  disp activate                          -> add to PATH\n";
        cout << "\nLog file: log.<basename>.displog (or use --log)\n";
    };

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        }

        if (arg == "--version") {
            cout << "disp 0.1.0\n";
            return 0;
        }

        if ((arg == "-v") || (arg == "--verbose")) {
            verbose = true;
            continue;
        }

        if (arg == "--log" && i + 1 < argc) {
            logFileOverride = argv[++i];
            continue;
        }

        if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
            continue;
        }

        if (arg == "-emit-c") { emitMode = "c"; continue; }
        if (arg == "-emit-asm") { emitMode = "asm"; continue; }
        if (arg == "-emit-exe") { emitMode = "exe"; continue; }
        if (arg == "-emit-disp") { emitMode = "disp"; continue; }
        if (arg == "-bare-asm") { emitMode = "bare-asm"; continue; }
        if (arg == "-emit-arm") { emitMode = "arm"; continue; }
        if (arg == "-emit-riscv") { emitMode = "riscv"; continue; }

        if (inputFile.empty()) {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        cerr << "Error: No input file specified.\n";
        return 1;
    }

    // Read input file
    ifstream infile(inputFile);
    if (!infile) {
        cerr << "Error: Cannot open file: " << inputFile << "\n";
        // write minimal log before exit
        string base = removeExtension(basename(inputFile));
        string logFile = logFileOverride.empty() ? ("log." + base + ".displog") : logFileOverride;
        ofstream logfile(logFile);
        if (logfile) {
            logfile << "=== Displexity Compilation Log ===\n";
            logfile << "Input: " << inputFile << "\n";
            logfile << "Status: Error\n";
            logfile << "Error: Cannot open input file.\n";
            logfile.close();
            cerr << "Log: " << logFile << "\n";
        }
        return 1;
    }

    stringstream buffer;
    buffer << infile.rdbuf();
    string source = buffer.str();
    infile.close();

    // Prepare log early so it's available for include resolution
    string baseName = removeExtension(basename(inputFile));
    string logFile = logFileOverride.empty() ? ("log." + baseName + ".displog") : logFileOverride;
    vector<string> logLines;
    auto addLog = [&](const string &s){ logLines.push_back(s); };
    // timestamp
    {
        time_t t = time(nullptr);
        struct tm tmv;
#ifdef _WIN32
        localtime_s(&tmv, &t);
#else
        localtime_r(&t, &tmv);
#endif
        stringstream ts;
        ts << put_time(&tmv, "%Y-%m-%d %H:%M:%S");
        addLog(string("Timestamp: ") + ts.str());
    }
    addLog(string("Input: ") + inputFile);

    // Resolve includes: search for include "name" patterns and attempt to locate .dish/.disll
    namespace fs = std::filesystem;
    auto locateIncludeFile = [&](const string &incName) -> string {
        // Try exact path first
        if (fs::exists(incName)) return incName;
        // Try with .dish
        string tryDish = incName;
        if (tryDish.find('.') == string::npos) tryDish += ".dish";
        if (fs::exists(tryDish)) return tryDish;
        // Search in input file dir
        fs::path inputDir = fs::path(inputFile).parent_path();
        fs::path p1 = inputDir / tryDish;
        if (fs::exists(p1)) return p1.string();
        // Search current working directory
        fs::path cwd = fs::current_path();
        fs::path p2 = cwd / tryDish;
        if (fs::exists(p2)) return p2.string();
        // Search workspace root (one level up from compiler directory if present)
        fs::path wsRoot = fs::path(".");
        try {
            for (auto &d : {inputDir, cwd, wsRoot}) {
                if (!fs::exists(d)) continue;
                // recursive search but limit matches
                int found = 0;
                for (auto it = fs::recursive_directory_iterator(d); it != fs::recursive_directory_iterator(); ++it) {
                    try {
                        if (!fs::is_regular_file(*it)) continue;
                        if (it->path().filename() == tryDish || it->path().filename() == incName) {
                            return it->path().string();
                        }
                        // also check .disll variants
                        if (it->path().extension() == ".disll" && it->path().stem() == fs::path(incName).stem()) {
                            return it->path().string();
                        }
                    } catch (...) { continue; }
                    if (++found > 500) break;
                }
            }
        } catch (...) {}
        // As a last resort, try scanning logical drives roots (limited)
        for (char drive = 'A'; drive <= 'Z'; ++drive) {
            string root = string(1, drive) + ":\\";
            try {
                if (!fs::exists(root)) continue;
                int found = 0;
                for (auto &p : fs::recursive_directory_iterator(root)) {
                    try {
                        if (!fs::is_regular_file(p)) continue;
                        if (p.path().filename() == tryDish) return p.path().string();
                    } catch (...) { }
                    if (++found > 200) break;
                }
            } catch (...) { continue; }
        }
        return string();
    };

    // Find include directives in source
    vector<pair<string,string>> includesFound; // pair<incName, resolvedPath>
    {
        std::istringstream ss(source);
        string line;
        while (std::getline(ss, line)) {
            // crude parse: look for include "..."
            auto pos = line.find("include");
            if (pos == string::npos) continue;
            auto q1 = line.find('"', pos);
            if (q1 == string::npos) continue;
            auto q2 = line.find('"', q1 + 1);
            if (q2 == string::npos) continue;
            string inc = line.substr(q1 + 1, q2 - q1 - 1);
            string resolved = locateIncludeFile(inc);
            includesFound.push_back({inc, resolved});
        }
    }

    // Prepend included files' contents to source (in order)
    string aggregatedIncludes;
    for (auto &pr : includesFound) {
        string inc = pr.first;
        string resolved = pr.second;
        if (!resolved.empty()) {
            try {
                ifstream incf(resolved);
                if (incf) {
                    stringstream sb;
                    sb << "// --- begin include: " << resolved << " ---\n";
                    sb << incf.rdbuf();
                    sb << "\n// --- end include: " << resolved << " ---\n";
                    aggregatedIncludes += sb.str();
                    addLog(string("Included: ") + resolved);
                    // if .disll, note library discovered
                    if (fs::path(resolved).extension() == ".disll") {
                        addLog(string("Library discovered: ") + resolved);
                    }
                }
            } catch (...) {}
        } else {
            addLog(string("Include not found: ") + inc);
        }
    }

    if (!aggregatedIncludes.empty()) {
        source = aggregatedIncludes + "\n" + source;
    }

    // Default output file (default is .exe if not specified)
    if (outputFile.empty()) {
        string base = removeExtension(basename(inputFile));
        if (emitMode == "c") outputFile = base + ".c";
        else if (emitMode == "asm") outputFile = base + ".asm";
        else if (emitMode == "disp") outputFile = base + ".disp";
        else if (emitMode == "bare-asm") outputFile = base + ".s";
        else if (emitMode == "arm") outputFile = base + ".arm.s";
        else if (emitMode == "riscv") outputFile = base + ".riscv.s";
        else outputFile = base + ".exe";  // default: emit exe
    }

    // Add output and mode to log
    addLog(string("Output: ") + outputFile);
    addLog(string("Mode: ") + emitMode);
    if (verbose) addLog("Verbose: true");

    // Lexical analysis
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    addLog(string("Tokens: ") + to_string(tokens.size()));

    // Parsing
    Parser parser(tokens);
    Program program;
    bool failed = false;
    string errorMessage;
    try {
        program = parser.parse();
    } catch (const exception& e) {
        cerr << "Parse error: " << e.what() << "\n";
        addLog(string("Parse error: ") + e.what());
        failed = true;
        errorMessage = e.what();
    }

    if (failed) {
        ofstream logfile(logFile);
        if (logfile) {
            logfile << "=== Displexity Compilation Log ===\n";
            for (auto &l : logLines) logfile << l << "\n";
            logfile << "Status: Error\n";
            logfile << "Error: " << errorMessage << "\n";
            logfile.close();
            cerr << "Log: " << logFile << "\n";
        }
        return 1;
    }

    // Code generation
    string generatedCode;
    if (emitMode == "asm") {
        AsmGenerator asmgen;
        generatedCode = asmgen.generate(program);
        // Write assembly output directly
        ofstream outfile(outputFile);
        if (!outfile) {
            cerr << "Error: Cannot write asm file: " << outputFile << "\n";
            addLog(string("Error: Cannot write asm file: ") + outputFile);
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: Cannot write asm file.\n";
                logfile.close();
            }
            return 1;
        }
        outfile << generatedCode;
        outfile.close();
        addLog(string("ASM emitted: ") + outputFile);
        cout << "ASM emitted: " << outputFile << "\n";
        // finalize log and exit
        addLog(string("Status: Success"));
        ofstream logfile(logFile);
        if (logfile) {
            logfile << "=== Displexity Compilation Log ===\n";
            for (auto &l : logLines) logfile << l << "\n";
            logfile.close();
        }
        return 0;
    } else if (emitMode == "bare-asm") {
        BiosGenerator biosgen;
        generatedCode = biosgen.generate(program);
        // Write BIOS assembly output directly
        ofstream outfile(outputFile);
        if (!outfile) {
            cerr << "Error: Cannot write asm file: " << outputFile << "\n";
            addLog(string("Error: Cannot write asm file: ") + outputFile);
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: Cannot write asm file.\n";
                logfile.close();
            }
            return 1;
        }
        outfile << generatedCode;
        outfile.close();
        addLog(string("BIOS asm emitted: ") + outputFile);
        cout << "BIOS asm emitted: " << outputFile << "\n";
        cout << "Assemble with: nasm -f bin " << outputFile << " -o " << removeExtension(outputFile) << ".bin\n";
        // finalize log and exit
        addLog(string("Status: Success"));
        ofstream logfile(logFile);
        if (logfile) {
            logfile << "=== Displexity Compilation Log ===\n";
            for (auto &l : logLines) logfile << l << "\n";
            logfile.close();
        }
        return 0;
    } else if (emitMode == "arm") {
        ArmGenerator armgen;
        generatedCode = armgen.generate(program);
        // Write ARM assembly output directly
        ofstream outfile(outputFile);
        if (!outfile) {
            cerr << "Error: Cannot write asm file: " << outputFile << "\n";
            addLog(string("Error: Cannot write asm file: ") + outputFile);
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: Cannot write asm file.\n";
                logfile.close();
            }
            return 1;
        }
        outfile << generatedCode;
        outfile.close();
        addLog(string("ARM64 asm emitted: ") + outputFile);
        cout << "ARM64 asm emitted: " << outputFile << "\n";
        cout << "Assemble with: aarch64-linux-gnu-as " << outputFile << " -o " << removeExtension(outputFile) << ".o\n";
        // finalize log and exit
        addLog(string("Status: Success"));
        ofstream logfile(logFile);
        if (logfile) {
            logfile << "=== Displexity Compilation Log ===\n";
            for (auto &l : logLines) logfile << l << "\n";
            logfile.close();
        }
        return 0;
    } else if (emitMode == "riscv") {
        RiscVGenerator riscvgen;
        generatedCode = riscvgen.generate(program);
        // Write RISC-V assembly output directly
        ofstream outfile(outputFile);
        if (!outfile) {
            cerr << "Error: Cannot write asm file: " << outputFile << "\n";
            addLog(string("Error: Cannot write asm file: ") + outputFile);
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: Cannot write asm file.\n";
                logfile.close();
            }
            return 1;
        }
        outfile << generatedCode;
        outfile.close();
        addLog(string("RISC-V asm emitted: ") + outputFile);
        cout << "RISC-V asm emitted: " << outputFile << "\n";
        cout << "Assemble with: riscv64-unknown-elf-as " << outputFile << " -o " << removeExtension(outputFile) << ".o\n";
        // finalize log and exit
        addLog(string("Status: Success"));
        ofstream logfile(logFile);
        if (logfile) {
            logfile << "=== Displexity Compilation Log ===\n";
            for (auto &l : logLines) logfile << l << "\n";
            logfile.close();
        }
        return 0;
    } else {
        CodeGenerator codegen;
        generatedCode = codegen.generate(program);
    }

    // Determine actual output strategy
    bool compileToExe = (emitMode == "exe") || (fs::path(outputFile).extension() == ".exe");
    bool createDispPackage = (emitMode == "disp") || (fs::path(outputFile).extension() == ".disp");
    
    // If compiling to exe or disp, write C to temp file and invoke GCC
    string tempCFile;
    if (compileToExe || createDispPackage) {
        tempCFile = removeExtension(outputFile) + "_temp.c";
        ofstream tempfile(tempCFile);
        if (!tempfile) {
            cerr << "Error: Cannot write temp file: " << tempCFile << "\n";
            addLog(string("Error: Cannot write temp C file: ") + tempCFile);
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: Cannot write temp C file.\n";
                logfile.close();
            }
            return 1;
        }
        tempfile << generatedCode;
        tempfile.close();
        addLog(string("Temp C file: ") + tempCFile);
    } else {
        // Write C output directly
        ofstream outfile(outputFile);
        if (!outfile) {
            cerr << "Error: Cannot write to file: " << outputFile << "\n";
            addLog(string("Error: Cannot write output file: ") + outputFile);
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: Cannot write output file.\n";
                logfile.close();
                cerr << "Log: " << logFile << "\n";
            }
            return 1;
        }
        outfile << generatedCode;
        outfile.close();
    }

    // If compiling to exe or disp, invoke GCC
    if (compileToExe || createDispPackage) {
        string exeName = removeExtension(outputFile);
        #ifdef _WIN32
            exeName += ".exe";
        #endif
        
        string gccCmd = string("gcc ") + tempCFile + " -o " + exeName;
        if (verbose) {
            cout << "Invoking: " << gccCmd << "\n";
            addLog(string("GCC command: ") + gccCmd);
        }
        
        int gccResult = system(gccCmd.c_str());
        if (gccResult != 0) {
            cerr << "Error: GCC compilation failed with code " << gccResult << "\n";
            addLog(string("GCC failed with code: ") + to_string(gccResult));
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: GCC compilation failed.\n";
                logfile.close();
                cerr << "Log: " << logFile << "\n";
            }
            return 1;
        }
        addLog(string("GCC succeeded: ") + exeName);
        
        // Clean up temp C file
        try {
            fs::remove(tempCFile);
        } catch (...) {}
        
        // If creating .disp package, create using proper binary format
        if (createDispPackage) {
            try {
                DispPackage::create(exeName, outputFile);
                addLog(string("Created .disp package: ") + outputFile);
            } catch (const exception &e) {
                cerr << "Error: Cannot create .disp package: " << e.what() << "\n";
                addLog(string("Error creating .disp: ") + e.what());
            }
        }
        
        cout << "Compiled: " << inputFile << " -> " << exeName << "\n";
        if (createDispPackage) cout << "Package: " << outputFile << "\n";
    }

    // Finalize log with success
    addLog(string("Status: Success"));
    ofstream logfile(logFile);
    if (logfile) {
        logfile << "=== Displexity Compilation Log ===\n";
        for (auto &l : logLines) logfile << l << "\n";
        logfile.close();
    }

    cout << "Log: " << logFile << "\n";
    return 0;
}
