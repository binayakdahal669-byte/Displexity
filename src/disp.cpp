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
#include "wasmgen.h"
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

// .disp package format: Custom Displexity Executable Format
// [MAGIC:8][VERSION:4][CHECKSUM:8][FLAGS:4][TIMESTAMP:8][BINARY_SIZE:8][METADATA_SIZE:4][METADATA][BINARY_DATA]
struct DispPackage {
    static const string MAGIC;
    
    static void create(const string &exePath, const string &dispPath) {
        ifstream exe(exePath, ios::binary);
        if (!exe) throw runtime_error("Cannot read exe: " + exePath);
        
        stringstream ss;
        ss << exe.rdbuf();
        string binaryData = ss.str();
        exe.close();
        
        // Calculate checksum
        uint64_t checksum = 0;
        for (char c : binaryData) {
            checksum = checksum * 31 + (unsigned char)c;
        }
        
        // Create metadata
        string metadata = "DISPLEXITY_EXECUTABLE\n";
        metadata += "CREATED_BY=DISP_COMPILER\n";
        metadata += "FORMAT_VERSION=2.0\n";
        metadata += "TAMPER_PROTECTION=ENABLED\n";
        
        ofstream disp(dispPath, ios::binary);
        if (!disp) throw runtime_error("Cannot write disp: " + dispPath);
        
        // Write custom header (completely different from MZ)
        disp << MAGIC;  // "DISPLX02" (8 bytes) - custom magic
        uint32_t version = 2;
        disp.write((char*)&version, 4);
        disp.write((char*)&checksum, 8);
        uint32_t flags = 0x44495350; // "DISP" in hex
        disp.write((char*)&flags, 4);
        uint64_t timestamp = time(nullptr);
        disp.write((char*)&timestamp, 8);
        uint64_t binSize = binaryData.size();
        disp.write((char*)&binSize, 8);
        uint32_t metaSize = metadata.size();
        disp.write((char*)&metaSize, 4);
        
        // Write metadata
        disp.write(metadata.c_str(), metadata.size());
        
        // Encrypt/obfuscate binary data (simple XOR for now)
        string obfuscatedData;
        uint8_t key = 0xD1; // Displexity key
        for (size_t i = 0; i < binaryData.size(); i++) {
            obfuscatedData += (char)(binaryData[i] ^ key ^ (i & 0xFF));
        }
        
        // Write obfuscated binary
        disp.write(obfuscatedData.c_str(), obfuscatedData.size());
        disp.close();
    }
    
    static void extract(const string &dispPath, const string &exePath) {
        ifstream disp(dispPath, ios::binary);
        if (!disp) throw runtime_error("Cannot read disp: " + dispPath);
        
        char magic[9] = {0};
        disp.read(magic, 8);
        if (string(magic) != MAGIC) {
            throw runtime_error("Invalid .disp file - not a Displexity executable");
        }
        
        uint32_t version;
        disp.read((char*)&version, 4);
        if (version != 2) {
            throw runtime_error("Unsupported .disp version");
        }
        
        uint64_t storedChecksum;
        disp.read((char*)&storedChecksum, 8);
        
        uint32_t flags;
        disp.read((char*)&flags, 4);
        if (flags != 0x44495350) {
            throw runtime_error("Corrupted .disp file - invalid flags");
        }
        
        uint64_t timestamp;
        disp.read((char*)&timestamp, 8);
        
        uint64_t binSize;
        disp.read((char*)&binSize, 8);
        
        uint32_t metaSize;
        disp.read((char*)&metaSize, 4);
        
        // Read and verify metadata
        vector<char> metadata(metaSize);
        disp.read(metadata.data(), metaSize);
        string metaStr(metadata.begin(), metadata.end());
        if (metaStr.find("DISPLEXITY_EXECUTABLE") == string::npos) {
            throw runtime_error("Invalid .disp metadata");
        }
        
        // Read obfuscated binary
        vector<char> obfuscatedBinary(binSize);
        disp.read(obfuscatedBinary.data(), binSize);
        disp.close();
        
        // Deobfuscate binary data
        string binaryData;
        uint8_t key = 0xD1;
        for (size_t i = 0; i < binSize; i++) {
            binaryData += (char)(obfuscatedBinary[i] ^ key ^ (i & 0xFF));
        }
        
        // Verify checksum
        uint64_t checksum = 0;
        for (char c : binaryData) {
            checksum = checksum * 31 + (unsigned char)c;
        }
        if (checksum != storedChecksum) {
            throw runtime_error("Corrupted .disp file - checksum mismatch");
        }
        
        // Check if trying to rename to .exe (tamper protection)
        if (exePath.find(".exe") != string::npos && 
            exePath != removeExtension(dispPath) + "_tmp_run.exe") {
            throw runtime_error("Tamper protection: .disp files cannot be renamed to .exe");
        }
        
        ofstream exe(exePath, ios::binary);
        if (!exe) throw runtime_error("Cannot write exe: " + exePath);
        exe.write(binaryData.c_str(), binaryData.size());
        exe.close();
    }
};
const string DispPackage::MAGIC = "DISPLX02";

// .disll library format: Enhanced Binary Library Format
// [MAGIC:8][VERSION:4][CHECKSUM:8][COMPRESSION:4][NUM_FUNCS:4][FUNC_ENTRIES...][CODE_SIZE:4][COMPRESSED_C_CODE]
// FUNC_ENTRY: [NAME_LEN:2][NAME][RETURN_TYPE_LEN:1][RETURN_TYPE][NUM_PARAMS:1][PARAMS...]
// PARAM: [NAME_LEN:1][NAME][TYPE_LEN:1][TYPE]
struct DispLibrary {
    static const string MAGIC;
    
    struct FuncInfo {
        string name;
        string returnType;
        vector<pair<string, string>> params;  // name, type
    };
    
    static void create(const string &sourcePath, const string &disllPath, const string &generatedC) {
        // Parse source to extract function signatures
        ifstream src(sourcePath);
        if (!src) throw runtime_error("Cannot read source: " + sourcePath);
        
        stringstream ss;
        ss << src.rdbuf();
        string source = ss.str();
        src.close();
        
        vector<FuncInfo> funcs;
        // Enhanced parser for func declarations
        size_t pos = 0;
        while ((pos = source.find("func ", pos)) != string::npos) {
            pos += 5;
            // Skip whitespace
            while (pos < source.size() && isspace(source[pos])) pos++;
            
            // Get function name
            size_t nameStart = pos;
            while (pos < source.size() && (isalnum(source[pos]) || source[pos] == '_')) pos++;
            string funcName = source.substr(nameStart, pos - nameStart);
            
            // Skip to (
            while (pos < source.size() && source[pos] != '(') pos++;
            if (pos >= source.size()) break;
            pos++;  // skip (
            
            // Parse parameters
            vector<pair<string, string>> params;
            while (pos < source.size() && source[pos] != ')') {
                // Skip whitespace
                while (pos < source.size() && isspace(source[pos])) pos++;
                if (source[pos] == ')') break;
                
                // Get type
                size_t typeStart = pos;
                while (pos < source.size() && isalnum(source[pos])) pos++;
                string paramType = source.substr(typeStart, pos - typeStart);
                
                // Skip whitespace
                while (pos < source.size() && isspace(source[pos])) pos++;
                
                // Get param name
                size_t pnameStart = pos;
                while (pos < source.size() && (isalnum(source[pos]) || source[pos] == '_')) pos++;
                string paramName = source.substr(pnameStart, pos - pnameStart);
                
                if (!paramType.empty() && !paramName.empty()) {
                    params.push_back({paramName, paramType});
                }
                
                // Skip comma
                while (pos < source.size() && (source[pos] == ',' || isspace(source[pos]))) pos++;
            }
            
            FuncInfo fi;
            fi.name = funcName;
            fi.returnType = "int";  // default
            fi.params = params;
            funcs.push_back(fi);
        }
        
        // Simple compression (RLE-like for repeated patterns)
        string compressedCode = compressCode(generatedC);
        
        // Calculate checksum
        uint64_t checksum = 0;
        for (char c : compressedCode) {
            checksum = checksum * 37 + (unsigned char)c;
        }
        for (const auto& f : funcs) {
            for (char c : f.name) {
                checksum = checksum * 37 + (unsigned char)c;
            }
        }
        
        // Write enhanced binary library
        ofstream lib(disllPath, ios::binary);
        if (!lib) throw runtime_error("Cannot write library: " + disllPath);
        
        // Enhanced header
        lib.write(MAGIC.c_str(), 8);
        uint32_t version = 2;
        lib.write((char*)&version, 4);
        lib.write((char*)&checksum, 8);
        uint32_t compression = 1; // Simple compression enabled
        lib.write((char*)&compression, 4);
        uint32_t numFuncs = funcs.size();
        lib.write((char*)&numFuncs, 4);
        
        // Function entries with enhanced metadata
        for (const auto& f : funcs) {
            uint16_t nameLen = f.name.size();
            lib.write((char*)&nameLen, 2);
            lib.write(f.name.c_str(), nameLen);
            
            uint8_t retLen = f.returnType.size();
            lib.write((char*)&retLen, 1);
            lib.write(f.returnType.c_str(), retLen);
            
            uint8_t numParams = f.params.size();
            lib.write((char*)&numParams, 1);
            for (const auto& p : f.params) {
                uint8_t pnLen = p.first.size();
                lib.write((char*)&pnLen, 1);
                lib.write(p.first.c_str(), pnLen);
                uint8_t ptLen = p.second.size();
                lib.write((char*)&ptLen, 1);
                lib.write(p.second.c_str(), ptLen);
            }
        }
        
        // Compressed C code
        uint32_t codeSize = compressedCode.size();
        lib.write((char*)&codeSize, 4);
        lib.write(compressedCode.c_str(), codeSize);
        
        lib.close();
    }
    
    static pair<vector<FuncInfo>, string> load(const string &disllPath) {
        ifstream lib(disllPath, ios::binary);
        if (!lib) throw runtime_error("Cannot read library: " + disllPath);
        
        char magic[9] = {0};
        lib.read(magic, 8);
        if (string(magic) != MAGIC) throw runtime_error("Invalid .disll file: " + disllPath);
        
        uint32_t version;
        lib.read((char*)&version, 4);
        if (version < 1 || version > 2) {
            throw runtime_error("Unsupported .disll version: " + to_string(version));
        }
        
        uint64_t storedChecksum = 0;
        uint32_t compression = 0;
        
        if (version >= 2) {
            lib.read((char*)&storedChecksum, 8);
            lib.read((char*)&compression, 4);
        }
        
        uint32_t numFuncs;
        lib.read((char*)&numFuncs, 4);
        
        vector<FuncInfo> funcs;
        for (uint32_t i = 0; i < numFuncs; i++) {
            FuncInfo fi;
            
            uint16_t nameLen;
            lib.read((char*)&nameLen, 2);
            fi.name.resize(nameLen);
            lib.read(&fi.name[0], nameLen);
            
            uint8_t retLen;
            lib.read((char*)&retLen, 1);
            fi.returnType.resize(retLen);
            lib.read(&fi.returnType[0], retLen);
            
            uint8_t numParams;
            lib.read((char*)&numParams, 1);
            for (uint8_t j = 0; j < numParams; j++) {
                uint8_t pnLen;
                lib.read((char*)&pnLen, 1);
                string pname(pnLen, '\0');
                lib.read(&pname[0], pnLen);
                
                uint8_t ptLen;
                lib.read((char*)&ptLen, 1);
                string ptype(ptLen, '\0');
                lib.read(&ptype[0], ptLen);
                
                fi.params.push_back({pname, ptype});
            }
            funcs.push_back(fi);
        }
        
        uint32_t codeSize;
        lib.read((char*)&codeSize, 4);
        string code(codeSize, '\0');
        lib.read(&code[0], codeSize);
        
        // Decompress if needed
        if (compression == 1) {
            code = decompressCode(code);
        }
        
        // Verify checksum for version 2+
        if (version >= 2) {
            uint64_t checksum = 0;
            for (char c : code) {
                checksum = checksum * 37 + (unsigned char)c;
            }
            for (const auto& f : funcs) {
                for (char c : f.name) {
                    checksum = checksum * 37 + (unsigned char)c;
                }
            }
            if (checksum != storedChecksum) {
                throw runtime_error("Corrupted .disll file - checksum mismatch");
            }
        }
        
        lib.close();
        return {funcs, code};
    }
    
private:
    static string compressCode(const string& input) {
        // Simple RLE compression for repeated patterns
        string result;
        if (input.empty()) return result;
        
        char current = input[0];
        int count = 1;
        
        for (size_t i = 1; i < input.size(); i++) {
            if (input[i] == current && count < 255) {
                count++;
            } else {
                if (count > 3) {
                    result += (char)0xFF; // Escape sequence
                    result += (char)count;
                    result += current;
                } else {
                    for (int j = 0; j < count; j++) {
                        result += current;
                    }
                }
                current = input[i];
                count = 1;
            }
        }
        
        // Handle last sequence
        if (count > 3) {
            result += (char)0xFF;
            result += (char)count;
            result += current;
        } else {
            for (int j = 0; j < count; j++) {
                result += current;
            }
        }
        
        return result;
    }
    
    static string decompressCode(const string& input) {
        string result;
        for (size_t i = 0; i < input.size(); i++) {
            if ((unsigned char)input[i] == 0xFF && i + 2 < input.size()) {
                int count = (unsigned char)input[i + 1];
                char ch = input[i + 2];
                for (int j = 0; j < count; j++) {
                    result += ch;
                }
                i += 2;
            } else {
                result += input[i];
            }
        }
        return result;
    }
};
const string DispLibrary::MAGIC = "DISLL002";

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
    vector<string> inputFiles;  // Additional input files for multi-file compilation
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
        cout << "  -emit-wasm             Emit WebAssembly text format (.wat)\n";
        cout << "  -emit-lib              Emit binary library (.disll)\n";
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
        cout << "  disp main.dis lib.dis -o app.exe       -> multi-file compilation\n";
        cout << "  disp hello.dis -emit-c -o hello.c      -> hello.c (C only)\n";
        cout << "  disp run hello.disp                    -> extract and run\n";
        cout << "  disp activate                          -> add to PATH\n";
        cout << "\nIDE: Run 'dispe' for the Neovim-based IDE\n";
        cout << "Log file: log.<basename>.displog (or use --log)\n";
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
        if (arg == "-emit-wasm") { emitMode = "wasm"; continue; }
        if (arg == "-emit-lib") { emitMode = "lib"; continue; }

        // Collect input files (support multiple .dis files)
        if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                // Additional input files - will be concatenated
                inputFiles.push_back(arg);
            }
        }
    }

    if (inputFile.empty()) {
        cerr << "Error: No input file specified.\n";
        return 1;
    }

    // Read all input files and concatenate
    string source;
    
    // Read main input file
    ifstream infile(inputFile);
    if (!infile) {
        cerr << "Error: Cannot open file: " << inputFile << "\n";
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
    source = buffer.str();
    infile.close();
    
    // Read additional input files
    for (const auto& extraFile : inputFiles) {
        ifstream extra(extraFile);
        if (extra) {
            source += "\n// --- File: " + extraFile + " ---\n";
            stringstream eb;
            eb << extra.rdbuf();
            source += eb.str();
            extra.close();
        } else {
            cerr << "Warning: Cannot open additional file: " << extraFile << "\n";
        }
    }

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
    string libraryCode;  // C code from binary libraries
    for (auto &pr : includesFound) {
        string inc = pr.first;
        string resolved = pr.second;
        if (!resolved.empty()) {
            try {
                // Check if it's a binary library (.disll)
                if (fs::path(resolved).extension() == ".disll") {
                    // Load binary library
                    try {
                        auto [funcs, code] = DispLibrary::load(resolved);
                        libraryCode += "// --- Library: " + resolved + " ---\n";
                        libraryCode += code;
                        libraryCode += "\n";
                        addLog(string("Loaded library: ") + resolved + " (" + to_string(funcs.size()) + " functions)");
                        for (const auto& f : funcs) {
                            addLog(string("  - ") + f.name + "()");
                        }
                    } catch (const exception& e) {
                        addLog(string("Error loading library ") + resolved + ": " + e.what());
                    }
                } else {
                    // Text include (.dis, .dish)
                    ifstream incf(resolved);
                    if (incf) {
                        stringstream sb;
                        sb << "// --- begin include: " << resolved << " ---\n";
                        sb << incf.rdbuf();
                        sb << "\n// --- end include: " << resolved << " ---\n";
                        aggregatedIncludes += sb.str();
                        addLog(string("Included: ") + resolved);
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
        else if (emitMode == "wasm") outputFile = base + ".wat";
        else if (emitMode == "lib") outputFile = base + ".disll";
        else outputFile = base + ".exe";  // default: emit exe
    }

    // Add output and mode to log
    addLog(string("Output: ") + outputFile);
    addLog(string("Mode: ") + emitMode);
    if (verbose) addLog("Verbose: true");

    // Split source into lines for error reporting
    vector<string> sourceLines;
    {
        istringstream ss(source);
        string line;
        while (getline(ss, line)) {
            sourceLines.push_back(line);
        }
    }

    // Lexical analysis
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    addLog(string("Tokens: ") + to_string(tokens.size()));
    
    // Check for lexer errors
    if (lexer.hasErrors()) {
        cerr << "\n\033[1;31m=== Lexer Errors ===\033[0m\n\n";
        lexer.printErrors();
        for (const auto& err : lexer.getErrors()) {
            addLog(string("Lexer error at line ") + to_string(err.line) + ": " + err.message);
        }
    }

    // Parsing
    Parser parser(tokens);
    parser.setSourceLines(sourceLines);
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
    
    // Check for parser errors
    if (parser.hasErrors()) {
        cerr << "\n\033[1;31m=== Syntax Errors ===\033[0m\n\n";
        parser.printErrors();
        for (const auto& err : parser.getErrors()) {
            addLog(string("Syntax error at line ") + to_string(err.line) + ": " + err.message);
        }
        failed = true;
        errorMessage = "Syntax errors found";
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
    } else if (emitMode == "wasm") {
        WasmGenerator wasmgen;
        generatedCode = wasmgen.generate(program);
        // Write WebAssembly text format output
        ofstream outfile(outputFile);
        if (!outfile) {
            cerr << "Error: Cannot write wat file: " << outputFile << "\n";
            addLog(string("Error: Cannot write wat file: ") + outputFile);
            ofstream logfile(logFile);
            if (logfile) {
                logfile << "=== Displexity Compilation Log ===\n";
                for (auto &l : logLines) logfile << l << "\n";
                logfile << "Status: Error\n";
                logfile << "Error: Cannot write wat file.\n";
                logfile.close();
            }
            return 1;
        }
        outfile << generatedCode;
        outfile.close();
        addLog(string("WebAssembly emitted: ") + outputFile);
        cout << "WebAssembly emitted: " << outputFile << "\n";
        cout << "Compile to .wasm with: wat2wasm " << outputFile << " -o " << removeExtension(outputFile) << ".wasm\n";
        cout << "Run with: wasmtime " << removeExtension(outputFile) << ".wasm\n";
        // finalize log and exit
        addLog(string("Status: Success"));
        ofstream logfile(logFile);
        if (logfile) {
            logfile << "=== Displexity Compilation Log ===\n";
            for (auto &l : logLines) logfile << l << "\n";
            logfile.close();
        }
        return 0;
    } else if (emitMode == "lib") {
        // Generate library - compile functions only, no main
        CodeGenerator codegen;
        generatedCode = codegen.generate(program);
        
        // Remove the main function from generated code for library
        size_t mainPos = generatedCode.find("\nint main(");
        if (mainPos != string::npos) {
            generatedCode = generatedCode.substr(0, mainPos);
        }
        
        // Strip #include statements - they'll be in the main file
        string libCode;
        istringstream iss(generatedCode);
        string line;
        bool pastIncludes = false;
        while (getline(iss, line)) {
            if (line.find("#include") == 0) continue;
            if (line.empty() && !pastIncludes) continue;
            pastIncludes = true;
            libCode += line + "\n";
        }
        
        try {
            DispLibrary::create(inputFile, outputFile, libCode);
            addLog(string("Library created: ") + outputFile);
            cout << "Library created: " << outputFile << "\n";
            cout << "Use with: include \"" << outputFile << "\"\n";
        } catch (const exception& e) {
            cerr << "Error creating library: " << e.what() << "\n";
            addLog(string("Error: ") + e.what());
            return 1;
        }
        
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

    // Inject library code into generated C (after includes, before main code)
    if (!libraryCode.empty()) {
        // Find position after #include statements and any blank lines
        size_t insertPos = 0;
        size_t lastInclude = generatedCode.rfind("#include");
        if (lastInclude != string::npos) {
            insertPos = generatedCode.find('\n', lastInclude);
            if (insertPos != string::npos) {
                insertPos++;
                // Skip blank lines
                while (insertPos < generatedCode.size() && 
                       (generatedCode[insertPos] == '\n' || generatedCode[insertPos] == '\r')) {
                    insertPos++;
                }
            }
        }
        
        // Insert library code (functions only, no includes)
        string libSection = "\n// === Linked Libraries ===\n" + libraryCode + "\n// === End Libraries ===\n\n";
        generatedCode.insert(insertPos, libSection);
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
        
        // Build GCC command with required libraries
        string gccCmd = string("gcc ") + tempCFile + " -o " + exeName;
        
        // Add icon resource on Windows
        string resourceFile;
        #ifdef _WIN32
        // Create resource file for icon
        resourceFile = removeExtension(outputFile) + "_res.rc";
        ofstream rcFile(resourceFile);
        if (rcFile) {
            // Look for icon in multiple locations
            string iconPath;
            vector<string> iconSearchPaths = {
                "exe.ico",                                    // Current directory
                "release/resources/exe.ico",                  // Dev release folder
                getenv("DISPLEXITY_HOME") ? string(getenv("DISPLEXITY_HOME")) + "/resources/exe.ico" : "",
                "C:/Program Files/Displexity/resources/exe.ico",
                "C:/Program Files (x86)/Displexity/resources/exe.ico"
            };
            
            for (const auto& path : iconSearchPaths) {
                if (!path.empty() && fs::exists(path)) {
                    iconPath = path;
                    break;
                }
            }
            
            if (!iconPath.empty()) {
                // Use forward slashes for RC file
                string rcIconPath = iconPath;
                for (char& c : rcIconPath) if (c == '\\') c = '/';
                rcFile << "1 ICON \"" << rcIconPath << "\"\n";
                addLog("Using icon: " + iconPath);
            } else {
                addLog("No icon file found, executable will have no icon");
            }
            rcFile.close();
            
            // Compile resource file only if we found an icon
            if (!iconPath.empty()) {
                string rcCmd = "windres " + resourceFile + " -o " + removeExtension(outputFile) + "_res.o";
                if (verbose) {
                    cout << "Compiling resource: " << rcCmd << "\n";
                }
                int rcResult = system(rcCmd.c_str());
                if (rcResult == 0) {
                    gccCmd += " " + removeExtension(outputFile) + "_res.o";
                    addLog("Icon resource compiled successfully");
                } else {
                    addLog("Warning: Resource compilation failed, continuing without icon");
                }
            }
        }
        #endif
        
        // Detect required libraries from generated code
        #ifdef _WIN32
        if (generatedCode.find("#include <wininet.h>") != string::npos) {
            gccCmd += " -lwininet";
        }
        if (generatedCode.find("#include <windows.h>") != string::npos && 
            generatedCode.find("CreateWindowA") != string::npos) {
            gccCmd += " -lgdi32 -luser32";
        }
        if (generatedCode.find("#include <gl/gl.h>") != string::npos) {
            gccCmd += " -lopengl32 -lgdi32 -luser32";
        }
        #else
        if (generatedCode.find("#include <curl/curl.h>") != string::npos) {
            gccCmd += " -lcurl";
        }
        if (generatedCode.find("glVertex") != string::npos) {
            gccCmd += " -lGL -lX11";
        }
        #endif
        
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
        
        // Clean up temp files
        try {
            fs::remove(tempCFile);
            #ifdef _WIN32
            if (!resourceFile.empty()) {
                fs::remove(resourceFile);
                fs::remove(removeExtension(outputFile) + "_res.o");
            }
            #endif
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
