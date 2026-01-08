/*
 * src/codegen_extended.h
 * Extended Code Generator for FFI, Web, Async, and Error Handling
 * Integrates interop.h, webfeatures.h, and runtime_enhanced.h features
 */

#ifndef CODEGEN_EXTENDED_H
#define CODEGEN_EXTENDED_H

#include <string>
#include <vector>
#include <map>
#include "ast.h"
#include "interop.h"
#include "webfeatures.h"

class CodeGenExtended {
public:
    std::string generateFFIDeclaration(const std::string& libName, 
                                       const std::vector<FFIFunction>& functions) {
        std::string code;
        code += "// FFI Library: " + libName + "\n";
        code += "#include \"displexity_ffi.h\"\n\n";
        
        code += "typedef struct {\n";
        code += "    void* lib_handle;\n";
        code += "    ffi_module_t* module;\n";
        code += "} " + libName + "_t;\n\n";
        
        code += libName + "_t* " + libName + "_init() {\n";
        code += "    " + libName + "_t* lib = malloc(sizeof(" + libName + "_t));\n";
        code += "    lib->lib_handle = dis_library_open(\"" + libName + "\");\n";
        code += "    if (!lib->lib_handle) return NULL;\n";
        code += "    return lib;\n";
        code += "}\n\n";
        
        for (const auto& func : functions) {
            code += generateFFIFunction(libName, func);
        }
        
        code += libName + "_cleanup(" + libName + "_t* lib) {\n";
        code += "    if (lib) {\n";
        code += "        dis_library_close(lib->lib_handle);\n";
        code += "        free(lib);\n";
        code += "    }\n";
        code += "}\n\n";
        
        return code;
    }
    
    std::string generateRESTRequest(const std::string& method,
                                    const std::string& url,
                                    const std::map<std::string, std::string>& headers,
                                    const std::string& body = "") {
        std::string code;
        code += "{\n";
        code += "    http_request_t req;\n";
        code += "    req.method = \"" + method + "\";\n";
        code += "    req.url = \"" + url + "\";\n";
        code += "    req.timeout = 30;\n";
        
        if (!headers.empty()) {
            code += "    req.headers = malloc(sizeof(http_header_t) * " + 
                    std::to_string(headers.size()) + ");\n";
            code += "    req.header_count = " + std::to_string(headers.size()) + ";\n";
            
            int i = 0;
            for (const auto& [key, value] : headers) {
                code += "    req.headers[" + std::to_string(i) + "].name = \"" + key + "\";\n";
                code += "    req.headers[" + std::to_string(i) + "].value = \"" + value + "\";\n";
                i++;
            }
        }
        
        if (!body.empty()) {
            code += "    req.body = \"" + body + "\";\n";
            code += "    req.body_length = strlen(req.body);\n";
        }
        
        code += "    http_response_t* resp = dis_http_request(&req);\n";
        code += "    if (resp) {\n";
        code += "        // Handle response\n";
        code += "        dis_http_response_free(resp);\n";
        code += "    }\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateWebSocket(const std::string& url) {
        std::string code;
        code += "{\n";
        code += "    websocket_t* ws = dis_websocket_create();\n";
        code += "    dis_websocket_connect(ws, \"" + url + "\");\n";
        code += "    \n";
        code += "    char buffer[1024];\n";
        code += "    while (dis_websocket_is_connected(ws)) {\n";
        code += "        int len = dis_websocket_receive(ws, buffer, sizeof(buffer));\n";
        code += "        if (len > 0) {\n";
        code += "            buffer[len] = '\\0';\n";
        code += "            // Process message\n";
        code += "        }\n";
        code += "    }\n";
        code += "    \n";
        code += "    dis_websocket_close(ws);\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateJSONValue(const std::string& jsonExpr) {
        std::string code;
        code += "{\n";
        code += "    json_value_t* json = dis_json_parse(\"" + jsonExpr + "\");\n";
        code += "    if (json) {\n";
        code += "        char* str = dis_json_to_string(json);\n";
        code += "        // Use json object\n";
        code += "        dis_json_free(json);\n";
        code += "    }\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateAsyncAwait(const std::string& asyncFunc) {
        std::string code;
        code += "{\n";
        code += "    promise_t* p = dis_promise_create();\n";
        code += "    // Execute async function\n";
        code += "    " + asyncFunc + "(p);\n";
        code += "    // Await result\n";
        code += "    void* result = dis_promise_await(p);\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateTryCatch(const std::string& tryBlock,
                                 const std::string& catchVar,
                                 const std::string& catchBlock,
                                 const std::string& finallyBlock = "") {
        std::string code;
        code += "{\n";
        code += "    error_handler_t* old_handler = dis_error_handler();\n";
        code += "    error_t* error = NULL;\n";
        code += "    \n";
        code += "    if (setjmp(dis_error_jump_buffer()) == 0) {\n";
        code += "        // Try block\n";
        code += "        " + tryBlock + "\n";
        code += "    } else {\n";
        code += "        // Catch block\n";
        code += "        error = dis_get_last_error();\n";
        code += "        " + catchBlock + "\n";
        code += "    }\n";
        
        if (!finallyBlock.empty()) {
            code += "    \n";
            code += "    // Finally block\n";
            code += "    " + finallyBlock + "\n";
        }
        
        code += "    \n";
        code += "    dis_restore_error_handler(old_handler);\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateServerEndpoint(const std::string& method,
                                       const std::string& route,
                                       const std::string& handler) {
        std::string code;
        code += "void " + method + "_" + sanitizeName(route) + "(http_request_t* req, http_response_t* resp) {\n";
        code += "    " + handler + "\n";
        code += "}\n";
        code += "\n";
        code += "// Register route\n";
        code += "dis_server_register_route(\"" + method + "\", \"" + route + "\", " +
                method + "_" + sanitizeName(route) + ");\n";
        
        return code;
    }
    
    std::string generateModuleLoad(const std::string& moduleName) {
        std::string code;
        code += "{\n";
        code += "    module_t* " + moduleName + " = dis_module_load(\"" + moduleName + "\", \"" + 
                moduleName + ".dis\");\n";
        code += "    if (!" + moduleName + ") {\n";
        code += "        dis_throw(\"Failed to load module: " + moduleName + "\");\n";
        code += "    }\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateTest(const std::string& testName, const std::string& testCode) {
        std::string code;
        code += "void test_" + sanitizeName(testName) + "() {\n";
        code += "    printf(\"Running test: " + testName + "\\n\");\n";
        code += "    " + testCode + "\n";
        code += "    printf(\"Test passed: " + testName + "\\n\");\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateAssert(const std::string& condition, const std::string& message = "") {
        std::string code;
        code += "if (!(" + condition + ")) {\n";
        code += "    fprintf(stderr, \"Assertion failed: ";
        if (!message.empty()) {
            code += message;
        } else {
            code += condition;
        }
        code += "\\n\");\n";
        code += "    exit(1);\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateGraphQL(const std::string& endpoint, const std::string& query) {
        std::string code;
        code += "{\n";
        code += "    graphql_client_t* client = dis_graphql_create(\"" + endpoint + "\");\n";
        code += "    const char* query = \"" + query + "\";\n";
        code += "    json_value_t* result = dis_graphql_query(client, query);\n";
        code += "    if (result) {\n";
        code += "        char* str = dis_json_to_string(result);\n";
        code += "        printf(\"GraphQL Result: %s\\n\", str);\n";
        code += "        dis_json_free(result);\n";
        code += "    }\n";
        code += "    dis_graphql_free(client);\n";
        code += "}\n";
        
        return code;
    }
    
    std::string generateUnsafeBlock(const std::string& code) {
        std::string result;
        result += "// UNSAFE BLOCK - Direct memory access\n";
        result += "{\n";
        result += "    " + code + "\n";
        result += "}\n";
        
        return result;
    }
    
private:
    std::string generateFFIFunction(const std::string& libName, const FFIFunction& func) {
        std::string code;
        
        // Generate wrapper function
        code += "// FFI: " + func.name + "\n";
        code += "typedef " + func.returnType + " (*ffi_" + func.name + "_t)(";
        
        for (size_t i = 0; i < func.paramTypes.size(); i++) {
            if (i > 0) code += ", ";
            code += func.paramTypes[i];
        }
        
        code += ");\n\n";
        
        code += func.returnType + " " + libName + "_" + func.name + "(";
        for (size_t i = 0; i < func.paramTypes.size(); i++) {
            if (i > 0) code += ", ";
            code += func.paramTypes[i] + " arg" + std::to_string(i);
        }
        code += ") {\n";
        
        code += "    ffi_" + func.name + "_t func_ptr = (ffi_" + func.name + "_t) ";
        code += "dis_library_get_function(dis_get_library(\"" + libName + "\"), \"" + func.name + "\");\n";
        code += "    if (!func_ptr) {\n";
        code += "        dis_throw(\"FFI function not found: " + func.name + "\");\n";
        code += "    }\n";
        
        if (func.returnType != "void") {
            code += "    return func_ptr(";
        } else {
            code += "    func_ptr(";
        }
        
        for (size_t i = 0; i < func.paramTypes.size(); i++) {
            if (i > 0) code += ", ";
            code += "arg" + std::to_string(i);
        }
        code += ");\n";
        code += "}\n\n";
        
        return code;
    }
    
    std::string sanitizeName(const std::string& name) {
        std::string result;
        for (char c : name) {
            if (isalnum(c) || c == '_') {
                result += c;
            } else {
                result += '_';
            }
        }
        return result;
    }
};

#endif // CODEGEN_EXTENDED_H
