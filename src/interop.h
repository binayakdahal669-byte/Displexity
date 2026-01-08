// Displexity FFI and Interop Module
// Allows Displexity to call C/C++ functions and external libraries
// File: src/interop.h

#ifndef DISPLEXITY_INTEROP_H
#define DISPLEXITY_INTEROP_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace disp {

// FFI Type system
enum class FFIType {
    VOID,
    INT,
    FLOAT,
    STRING,
    POINTER,
    FUNCTION,
    UNKNOWN
};

// FFI Function signature
struct FFIFunction {
    std::string name;
    std::string library;
    FFIType returnType;
    std::vector<std::pair<std::string, FFIType>> parameters;
    std::string cDeclaration;
    bool isVarArgs;
    
    FFIFunction(const std::string& n, const std::string& lib, FFIType ret)
        : name(n), library(lib), returnType(ret), isVarArgs(false) {}
};

// FFI Type registry
class FFITypeRegistry {
private:
    std::map<std::string, FFIType> typeMap;
    std::map<std::string, std::string> customTypes;
    
public:
    FFITypeRegistry() {
        // Register built-in types
        typeMap["void"] = FFIType::VOID;
        typeMap["int"] = FFIType::INT;
        typeMap["float"] = FFIType::FLOAT;
        typeMap["string"] = FFIType::STRING;
        typeMap["char*"] = FFIType::STRING;
        typeMap["const char*"] = FFIType::STRING;
        typeMap["void*"] = FFIType::POINTER;
    }
    
    FFIType getType(const std::string& typeName) {
        if (typeMap.find(typeName) != typeMap.end()) {
            return typeMap[typeName];
        }
        if (customTypes.find(typeName) != customTypes.end()) {
            // Return custom type mapping
            return FFIType::POINTER;  // Custom types are opaque pointers
        }
        return FFIType::UNKNOWN;
    }
    
    void registerCustomType(const std::string& name, const std::string& cType) {
        customTypes[name] = cType;
    }
};

// Module/Library loader
class Module {
public:
    std::string name;
    std::string path;
    std::string language;  // "c", "cpp", "dll", etc.
    std::vector<FFIFunction> functions;
    std::map<std::string, std::string> constants;
    
    Module(const std::string& n, const std::string& p)
        : name(n), path(p), language("c") {}
    
    void addFunction(const FFIFunction& func) {
        functions.push_back(func);
    }
    
    void addConstant(const std::string& name, const std::string& value) {
        constants[name] = value;
    }
    
    FFIFunction* getFunction(const std::string& name) {
        for (auto& func : functions) {
            if (func.name == name) return &func;
        }
        return nullptr;
    }
};

// Module registry
class ModuleRegistry {
private:
    std::map<std::string, std::shared_ptr<Module>> modules;
    FFITypeRegistry typeRegistry;
    
public:
    void registerModule(const std::shared_ptr<Module>& module) {
        modules[module->name] = module;
    }
    
    std::shared_ptr<Module> getModule(const std::string& name) {
        if (modules.find(name) != modules.end()) {
            return modules[name];
        }
        return nullptr;
    }
    
    FFIFunction* resolveFunction(const std::string& moduleName, const std::string& funcName) {
        auto module = getModule(moduleName);
        if (module) {
            return module->getFunction(funcName);
        }
        return nullptr;
    }
    
    FFITypeRegistry& getTypeRegistry() {
        return typeRegistry;
    }
};

// Standard library modules
class StdlibModules {
public:
    static std::shared_ptr<Module> createMathModule() {
        auto module = std::make_shared<Module>("math", "libm");
        
        FFIFunction sin("sin", "libm", FFIType::FLOAT);
        sin.parameters.push_back({"x", FFIType::FLOAT});
        sin.cDeclaration = "double sin(double x)";
        module->addFunction(sin);
        
        FFIFunction cos("cos", "libm", FFIType::FLOAT);
        cos.parameters.push_back({"x", FFIType::FLOAT});
        cos.cDeclaration = "double cos(double x)";
        module->addFunction(cos);
        
        FFIFunction sqrt("sqrt", "libm", FFIType::FLOAT);
        sqrt.parameters.push_back({"x", FFIType::FLOAT});
        sqrt.cDeclaration = "double sqrt(double x)";
        module->addFunction(sqrt);
        
        return module;
    }
    
    static std::shared_ptr<Module> createStringModule() {
        auto module = std::make_shared<Module>("string", "libc");
        
        FFIFunction strlen_func("strlen", "libc", FFIType::INT);
        strlen_func.parameters.push_back({"str", FFIType::STRING});
        strlen_func.cDeclaration = "size_t strlen(const char* str)";
        module->addFunction(strlen_func);
        
        FFIFunction strcpy_func("strcpy", "libc", FFIType::STRING);
        strcpy_func.parameters.push_back({"dest", FFIType::STRING});
        strcpy_func.parameters.push_back({"src", FFIType::STRING});
        strcpy_func.cDeclaration = "char* strcpy(char* dest, const char* src)";
        module->addFunction(strcpy_func);
        
        return module;
    }
    
    static std::shared_ptr<Module> createSocketModule() {
        auto module = std::make_shared<Module>("socket", "libsocket");
        
        FFIFunction socket_func("socket", "libsocket", FFIType::INT);
        socket_func.parameters.push_back({"domain", FFIType::INT});
        socket_func.parameters.push_back({"type", FFIType::INT});
        socket_func.parameters.push_back({"protocol", FFIType::INT});
        socket_func.cDeclaration = "int socket(int domain, int type, int protocol)";
        module->addFunction(socket_func);
        
        // Constants
        module->addConstant("AF_INET", "2");
        module->addConstant("SOCK_STREAM", "1");
        module->addConstant("IPPROTO_TCP", "6");
        
        return module;
    }
};

} // namespace disp

#endif // DISPLEXITY_INTEROP_H
