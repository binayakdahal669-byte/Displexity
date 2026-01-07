#ifndef DISPLEXITY_CTRANSPILE_H
#define DISPLEXITY_CTRANSPILE_H

#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include <map>

namespace disp {

class CTranspiler {
private:
    std::string source;
    std::stringstream output;
    int indentLevel = 0;
    std::map<std::string, std::string> typeMap;
    
    std::string indent() {
        return std::string(indentLevel * 4, ' ');
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }
    
    std::string convertType(const std::string& cType) {
        std::string t = trim(cType);
        if (t == "int" || t == "long" || t == "short") return "int";
        if (t == "float" || t == "double") return "float";
        if (t == "char" || t == "char*" || t == "const char*") return "str";
        if (t == "bool" || t == "_Bool") return "bool";
        if (t == "void") return "void";
        return "int"; // Default
    }
    
    std::string convertExpression(const std::string& expr) {
        std::string result = expr;
        
        // Convert printf to print
        std::regex printfRegex(R"(printf\s*\(\s*\"([^\"]*)\"\s*\))");
        result = std::regex_replace(result, printfRegex, "print \"$1\"");
        
        // Convert printf with format specifiers
        std::regex printfFmtRegex(R"(printf\s*\(\s*\"([^\"]*)\"\s*,\s*([^)]+)\))");
        std::smatch match;
        if (std::regex_search(result, match, printfFmtRegex)) {
            std::string fmt = match[1].str();
            std::string args = match[2].str();
            // Simple conversion - just print the variable
            result = "print " + trim(args);
        }
        
        // Convert puts
        std::regex putsRegex(R"(puts\s*\(\s*\"([^\"]*)\"\s*\))");
        result = std::regex_replace(result, putsRegex, "print \"$1\"");
        
        // Convert scanf to input
        std::regex scanfRegex(R"(scanf\s*\(\s*\"%[difs]\"\s*,\s*&(\w+)\s*\))");
        result = std::regex_replace(result, scanfRegex, "input $1");
        
        // Convert gets/fgets to input
        std::regex getsRegex(R"(gets\s*\(\s*(\w+)\s*\))");
        result = std::regex_replace(result, getsRegex, "input $1");
        
        // Convert == to is (optional, both work in Displexity)
        // result = std::regex_replace(result, std::regex("=="), " is ");
        
        // Convert != to isnt (optional)
        // result = std::regex_replace(result, std::regex("!="), " isnt ");
        
        // Convert && to and
        result = std::regex_replace(result, std::regex("&&"), " and ");
        
        // Convert || to or
        result = std::regex_replace(result, std::regex("\\|\\|"), " or ");
        
        // Convert ! to not (careful with !=)
        std::regex notRegex(R"(!\s*(\w+))");
        result = std::regex_replace(result, notRegex, "not $1");
        
        return result;
    }
    
    void processLine(const std::string& line) {
        std::string trimmed = trim(line);
        if (trimmed.empty()) {
            output << "\n";
            return;
        }
        
        // Skip preprocessor directives
        if (trimmed[0] == '#') {
            if (trimmed.find("#include") == 0) {
                // Convert to Displexity include if it's a user header
                std::regex includeRegex(R"(#include\s*\"([^\"]+)\")");
                std::smatch match;
                if (std::regex_search(trimmed, match, includeRegex)) {
                    std::string header = match[1].str();
                    // Convert .h to .dish
                    if (header.size() > 2 && header.substr(header.size()-2) == ".h") {
                        header = header.substr(0, header.size()-2) + ".dish";
                    }
                    output << indent() << "include \"" << header << "\"\n";
                }
                // Skip system includes
            }
            return;
        }
        
        // Skip comments but preserve them
        if (trimmed.substr(0, 2) == "//") {
            output << indent() << trimmed << "\n";
            return;
        }
        if (trimmed.substr(0, 2) == "/*") {
            output << indent() << trimmed << "\n";
            return;
        }
        
        // Handle closing braces
        if (trimmed == "}" || trimmed == "};") {
            if (indentLevel > 0) indentLevel--;
            output << indent() << "}\n";
            return;
        }
        
        // Handle function definitions
        std::regex funcRegex(R"((\w+)\s+(\w+)\s*\(([^)]*)\)\s*\{?)");
        std::smatch funcMatch;
        if (std::regex_search(trimmed, funcMatch, funcRegex)) {
            std::string retType = funcMatch[1].str();
            std::string funcName = funcMatch[2].str();
            std::string params = funcMatch[3].str();
            
            // Skip main's argc/argv
            if (funcName == "main") {
                params = "";
            }
            
            // Convert parameters
            std::string dispParams;
            if (!params.empty() && params != "void") {
                std::vector<std::string> paramList;
                std::stringstream ss(params);
                std::string param;
                while (std::getline(ss, param, ',')) {
                    param = trim(param);
                    // Parse "type name" or "type* name"
                    size_t lastSpace = param.find_last_of(" *");
                    if (lastSpace != std::string::npos) {
                        std::string ptype = trim(param.substr(0, lastSpace));
                        std::string pname = trim(param.substr(lastSpace + 1));
                        paramList.push_back(convertType(ptype) + " " + pname);
                    }
                }
                for (size_t i = 0; i < paramList.size(); i++) {
                    if (i > 0) dispParams += ", ";
                    dispParams += paramList[i];
                }
            }
            
            output << indent() << "func " << funcName << "(" << dispParams << ") {\n";
            indentLevel++;
            return;
        }
        
        // Handle variable declarations
        std::regex varDeclRegex(R"((\w+)\s+(\w+)\s*=\s*(.+);)");
        std::smatch varMatch;
        if (std::regex_search(trimmed, varMatch, varDeclRegex)) {
            std::string varType = varMatch[1].str();
            std::string varName = varMatch[2].str();
            std::string varValue = varMatch[3].str();
            output << indent() << convertType(varType) << " " << varName << " = " 
                   << convertExpression(varValue) << "\n";
            return;
        }
        
        // Handle variable declarations without initialization
        std::regex varDeclNoInitRegex(R"((\w+)\s+(\w+)\s*;)");
        if (std::regex_search(trimmed, varMatch, varDeclNoInitRegex)) {
            std::string varType = varMatch[1].str();
            std::string varName = varMatch[2].str();
            std::string defaultVal = "0";
            if (convertType(varType) == "str") defaultVal = "\"\"";
            if (convertType(varType) == "bool") defaultVal = "false";
            output << indent() << convertType(varType) << " " << varName << " = " << defaultVal << "\n";
            return;
        }
        
        // Handle if statements
        std::regex ifRegex(R"(if\s*\((.+)\)\s*\{?)");
        std::smatch ifMatch;
        if (std::regex_search(trimmed, ifMatch, ifRegex)) {
            std::string condition = ifMatch[1].str();
            output << indent() << "if " << convertExpression(condition) << " {\n";
            indentLevel++;
            return;
        }
        
        // Handle else if
        std::regex elifRegex(R"(else\s+if\s*\((.+)\)\s*\{?)");
        if (std::regex_search(trimmed, elifRegex)) {
            if (indentLevel > 0) indentLevel--;
            std::smatch m;
            std::regex_search(trimmed, m, elifRegex);
            output << indent() << "} elif " << convertExpression(m[1].str()) << " {\n";
            indentLevel++;
            return;
        }
        
        // Handle else
        if (trimmed == "else" || trimmed == "else {") {
            if (indentLevel > 0) indentLevel--;
            output << indent() << "} else {\n";
            indentLevel++;
            return;
        }
        
        // Handle while loops
        std::regex whileRegex(R"(while\s*\((.+)\)\s*\{?)");
        std::smatch whileMatch;
        if (std::regex_search(trimmed, whileMatch, whileRegex)) {
            std::string condition = whileMatch[1].str();
            output << indent() << "while " << convertExpression(condition) << " {\n";
            indentLevel++;
            return;
        }
        
        // Handle for loops
        std::regex forRegex(R"(for\s*\(\s*(\w+)\s*=\s*(\d+)\s*;\s*\w+\s*[<>=]+\s*(\d+)\s*;\s*\w+\+\+\s*\)\s*\{?)");
        std::smatch forMatch;
        if (std::regex_search(trimmed, forMatch, forRegex)) {
            std::string varName = forMatch[1].str();
            std::string start = forMatch[2].str();
            std::string end = forMatch[3].str();
            output << indent() << "for " << varName << " in " << start << ".." << end << " {\n";
            indentLevel++;
            return;
        }
        
        // Handle for loops with int declaration
        std::regex forIntRegex(R"(for\s*\(\s*int\s+(\w+)\s*=\s*(\d+)\s*;\s*\w+\s*[<>=]+\s*(\d+)\s*;\s*\w+\+\+\s*\)\s*\{?)");
        if (std::regex_search(trimmed, forMatch, forIntRegex)) {
            std::string varName = forMatch[1].str();
            std::string start = forMatch[2].str();
            std::string end = forMatch[3].str();
            output << indent() << "for " << varName << " in " << start << ".." << end << " {\n";
            indentLevel++;
            return;
        }
        
        // Handle return statements
        std::regex returnRegex(R"(return\s+(.+);)");
        std::smatch returnMatch;
        if (std::regex_search(trimmed, returnMatch, returnRegex)) {
            output << indent() << "return " << convertExpression(returnMatch[1].str()) << "\n";
            return;
        }
        if (trimmed == "return;" || trimmed == "return 0;") {
            output << indent() << "return 0\n";
            return;
        }
        
        // Handle assignments
        std::regex assignRegex(R"((\w+)\s*=\s*(.+);)");
        std::smatch assignMatch;
        if (std::regex_search(trimmed, assignMatch, assignRegex)) {
            output << indent() << assignMatch[1].str() << " = " 
                   << convertExpression(assignMatch[2].str()) << "\n";
            return;
        }
        
        // Handle increment/decrement
        std::regex incRegex(R"((\w+)\+\+;?)");
        std::smatch incMatch;
        if (std::regex_search(trimmed, incMatch, incRegex)) {
            output << indent() << incMatch[1].str() << " = " << incMatch[1].str() << " + 1\n";
            return;
        }
        
        std::regex decRegex(R"((\w+)--;?)");
        if (std::regex_search(trimmed, incMatch, decRegex)) {
            output << indent() << incMatch[1].str() << " = " << incMatch[1].str() << " - 1\n";
            return;
        }
        
        // Handle function calls (statements)
        std::regex callRegex(R"((\w+)\s*\(([^)]*)\)\s*;)");
        std::smatch callMatch;
        if (std::regex_search(trimmed, callMatch, callRegex)) {
            std::string funcName = callMatch[1].str();
            std::string args = callMatch[2].str();
            
            // Special handling for common C functions
            if (funcName == "printf") {
                output << indent() << convertExpression(trimmed) << "\n";
            } else if (funcName == "scanf") {
                output << indent() << convertExpression(trimmed) << "\n";
            } else if (funcName == "puts") {
                output << indent() << convertExpression(trimmed) << "\n";
            } else {
                output << indent() << funcName << "(" << args << ")\n";
            }
            return;
        }
        
        // Default: output as-is with semicolon removed
        if (!trimmed.empty() && trimmed.back() == ';') {
            trimmed = trimmed.substr(0, trimmed.size() - 1);
        }
        output << indent() << convertExpression(trimmed) << "\n";
    }

public:
    std::string transpile(const std::string& cSource) {
        source = cSource;
        output.str("");
        indentLevel = 0;
        
        output << "// Transpiled from C to Displexity\n";
        output << "// Some manual adjustments may be needed\n\n";
        
        std::istringstream stream(source);
        std::string line;
        
        while (std::getline(stream, line)) {
            processLine(line);
        }
        
        return output.str();
    }
    
    // Transpile a file
    static std::string transpileFile(const std::string& inputPath) {
        std::ifstream file(inputPath);
        if (!file) {
            return "// Error: Cannot open file: " + inputPath + "\n";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        
        CTranspiler transpiler;
        return transpiler.transpile(buffer.str());
    }
};

} // namespace disp

#endif // DISPLEXITY_CTRANSPILE_H
