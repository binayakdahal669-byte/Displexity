#ifndef DISPLEXITY_LEXER_H
#define DISPLEXITY_LEXER_H

#include <string>
#include <vector>
#include <cctype>
#include <iostream>
#include <sstream>

namespace disp {

enum class TokenType {
    // Literals
    INT, FLOAT, STRING, TRUE, FALSE,
    // Keywords
    PRINT, INPUT, IF, ELSE, WHILE, FOR, FUNC, CLASS, RETURN, INCLUDE, ARRAY,
    HTTP, KEYPRESS, HOTKEY, AI, GRAPHICS, CLEAR, DRAW, RENDER, WINDOW, FROM,
    FOR_THIS_USE_ICON, LOAD_IMAGE, DRAW_IMAGE, LOAD_SOUND, PLAY_SOUND, STOP_SOUND,
    GL_INIT, SET_COLOR, SET_CLEAR_COLOR, DRAW_TRIANGLE, DRAW_QUAD, FILL_RECT, 
    DRAW_CIRCLE, FILL_CIRCLE, SET_PROJECTION_2D, SET_PROJECTION_3D,
    TRANSLATE, ROTATE, SCALE, PUSH_MATRIX, POP_MATRIX, WINDOW_SHOULD_CLOSE,
    // Type keywords for declarations and conversions
    TYPE_INT, TYPE_STR, TYPE_FLOAT, TYPE_BOOL, TYPE_VAR,
    // Format specifiers for output
    FMT_BIN, FMT_HEX, FMT_OCT, FMT_DEC,
    // Identifiers and operators
    IDENT, PLUS, MINUS, STAR, SLASH, PERCENT, EQ, EQEQ, NEQ, LT, LE, GT, GE, LAND, LOR, NOT,
    // Stream operators
    SHL, SHR,  // << and >>
    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, SEMICOLON, COMMA, DOT, COLON, ARROW,
    // Special
    END_OF_FILE, UNKNOWN, ERROR
};
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

struct LexerError {
    std::string message;
    int line;
    int column;
    std::string context;
};

class Lexer {
private:
    std::string source;
    std::vector<std::string> lines;
    size_t current = 0;
    int line = 1;
    int column = 1;
    std::vector<LexerError> errors;

    void splitLines() {
        std::istringstream stream(source);
        std::string l;
        while (std::getline(stream, l)) {
            lines.push_back(l);
        }
    }

    char peek() const {
        if (current >= source.length()) return '\0';
        return source[current];
    }

    char peekNext() const {
        if (current + 1 >= source.length()) return '\0';
        return source[current + 1];
    }

    char advance() {
        char c = source[current++];
        if (c == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return c;
    }

    void skipWhitespace() {
        while (std::isspace(peek())) advance();
    }

    void skipComment() {
        if (peek() == '/' && peekNext() == '/') {
            advance(); advance();
            while (peek() != '\n' && peek() != '\0') advance();
        }
        // Multi-line comments
        if (peek() == '/' && peekNext() == '*') {
            int startLine = line;
            advance(); advance();
            while (!(peek() == '*' && peekNext() == '/') && peek() != '\0') {
                advance();
            }
            if (peek() == '\0') {
                addError("Unterminated multi-line comment starting at line " + std::to_string(startLine));
            } else {
                advance(); advance();
            }
        }
    }

    Token makeToken(TokenType type, const std::string& lexeme) {
        return {type, lexeme, line, column - (int)lexeme.length()};
    }

    void addError(const std::string& msg) {
        LexerError err;
        err.message = msg;
        err.line = line;
        err.column = column;
        if (line > 0 && line <= (int)lines.size()) {
            err.context = lines[line - 1];
        }
        errors.push_back(err);
    }

    Token scanString() {
        int startLine = line;
        int startCol = column;
        advance(); // opening quote
        std::string value;
        while (peek() != '"' && peek() != '\0' && peek() != '\n') {
            if (peek() == '\\') {
                advance();
                switch (peek()) {
                    case 'n': value += '\n'; advance(); break;
                    case 't': value += '\t'; advance(); break;
                    case 'r': value += '\r'; advance(); break;
                    case '\\': value += '\\'; advance(); break;
                    case '"': value += '"'; advance(); break;
                    case '0': value += '\0'; advance(); break;
                    default: 
                        addError("Unknown escape sequence: \\" + std::string(1, peek()));
                        value += peek(); 
                        advance();
                }
            } else {
                value += peek();
                advance();
            }
        }
        if (peek() == '\0' || peek() == '\n') {
            addError("Unterminated string literal starting at line " + std::to_string(startLine) + ", column " + std::to_string(startCol));
            return {TokenType::ERROR, value, startLine, startCol};
        }
        advance(); // closing quote
        return makeToken(TokenType::STRING, value);
    }

    Token scanNumber() {
        std::string value;
        bool hasDecimal = false;
        while (std::isdigit(peek()) || peek() == '.') {
            if (peek() == '.') {
                if (hasDecimal) {
                    addError("Invalid number: multiple decimal points");
                    break;
                }
                hasDecimal = true;
            }
            value += peek();
            advance();
        }
        return makeToken(hasDecimal ? TokenType::FLOAT : TokenType::INT, value);
    }

    Token scanIdentifier() {
        std::string value;
        while (std::isalnum(peek()) || peek() == '_') {
            value += peek();
            advance();
        }
        
        // Keywords (case-sensitive)
        if (value == "print") return makeToken(TokenType::PRINT, value);
        if (value == "say") return makeToken(TokenType::PRINT, value);
        if (value == "show") return makeToken(TokenType::PRINT, value);
        if (value == "input") return makeToken(TokenType::INPUT, value);
        if (value == "ask") return makeToken(TokenType::INPUT, value);
        if (value == "get") return makeToken(TokenType::INPUT, value);
        if (value == "read") return makeToken(TokenType::INPUT, value);
        if (value == "readline") return makeToken(TokenType::INPUT, value);
        if (value == "if") return makeToken(TokenType::IF, value);
        if (value == "when") return makeToken(TokenType::IF, value);
        if (value == "else") return makeToken(TokenType::ELSE, value);
        if (value == "otherwise") return makeToken(TokenType::ELSE, value);
        if (value == "while") return makeToken(TokenType::WHILE, value);
        if (value == "loop") return makeToken(TokenType::WHILE, value);
        if (value == "for") return makeToken(TokenType::FOR, value);
        if (value == "repeat") return makeToken(TokenType::FOR, value);
        if (value == "func") return makeToken(TokenType::FUNC, value);
        if (value == "function") return makeToken(TokenType::FUNC, value);
        if (value == "def") return makeToken(TokenType::FUNC, value);
        if (value == "class") return makeToken(TokenType::CLASS, value);
        if (value == "return") return makeToken(TokenType::RETURN, value);
        if (value == "give") return makeToken(TokenType::RETURN, value);
        if (value == "include") return makeToken(TokenType::INCLUDE, value);
        if (value == "import") return makeToken(TokenType::INCLUDE, value);
        if (value == "use") return makeToken(TokenType::INCLUDE, value);
        if (value == "from") return makeToken(TokenType::FROM, value);
        if (value == "for_this_use_icon") return makeToken(TokenType::FOR_THIS_USE_ICON, value);
        if (value == "set_icon") return makeToken(TokenType::FOR_THIS_USE_ICON, value);
        if (value == "app_icon") return makeToken(TokenType::FOR_THIS_USE_ICON, value);
        if (value == "load_image") return makeToken(TokenType::LOAD_IMAGE, value);
        if (value == "image") return makeToken(TokenType::LOAD_IMAGE, value);
        if (value == "draw_image") return makeToken(TokenType::DRAW_IMAGE, value);
        if (value == "blit") return makeToken(TokenType::DRAW_IMAGE, value);
        if (value == "load_sound") return makeToken(TokenType::LOAD_SOUND, value);
        if (value == "sound") return makeToken(TokenType::LOAD_SOUND, value);
        if (value == "audio") return makeToken(TokenType::LOAD_SOUND, value);
        if (value == "play_sound") return makeToken(TokenType::PLAY_SOUND, value);
        if (value == "play") return makeToken(TokenType::PLAY_SOUND, value);
        if (value == "stop_sound") return makeToken(TokenType::STOP_SOUND, value);
        if (value == "true") return makeToken(TokenType::TRUE, value);
        if (value == "yes") return makeToken(TokenType::TRUE, value);
        if (value == "false") return makeToken(TokenType::FALSE, value);
        if (value == "no") return makeToken(TokenType::FALSE, value);
        if (value == "Array") return makeToken(TokenType::ARRAY, value);
        if (value == "http") return makeToken(TokenType::HTTP, value);
        if (value == "web") return makeToken(TokenType::HTTP, value);
        if (value == "keypress") return makeToken(TokenType::KEYPRESS, value);
        if (value == "key") return makeToken(TokenType::KEYPRESS, value);
        if (value == "get_keypress") return makeToken(TokenType::KEYPRESS, value);
        if (value == "hotkey") return makeToken(TokenType::HOTKEY, value);
        if (value == "register_hotkey") return makeToken(TokenType::HOTKEY, value);
        if (value == "ai") return makeToken(TokenType::AI, value);
        if (value == "ai_query") return makeToken(TokenType::AI, value);
        if (value == "graphics") return makeToken(TokenType::GRAPHICS, value);
        if (value == "gfx") return makeToken(TokenType::GRAPHICS, value);
        if (value == "clear") return makeToken(TokenType::CLEAR, value);
        if (value == "cls") return makeToken(TokenType::CLEAR, value);
        if (value == "draw") return makeToken(TokenType::DRAW, value);
        if (value == "draw_pixel") return makeToken(TokenType::DRAW, value);
        if (value == "pixel") return makeToken(TokenType::DRAW, value);
        if (value == "draw_line") return makeToken(TokenType::DRAW, value);
        if (value == "line") return makeToken(TokenType::DRAW, value);
        if (value == "draw_rect") return makeToken(TokenType::DRAW, value);
        if (value == "rect") return makeToken(TokenType::DRAW, value);
        if (value == "set_color") return makeToken(TokenType::SET_COLOR, value);
        if (value == "color") return makeToken(TokenType::SET_COLOR, value);
        if (value == "render") return makeToken(TokenType::RENDER, value);
        if (value == "flip") return makeToken(TokenType::RENDER, value);
        if (value == "swap") return makeToken(TokenType::RENDER, value);
        if (value == "window") return makeToken(TokenType::WINDOW, value);
        if (value == "win") return makeToken(TokenType::WINDOW, value);
        if (value == "window_create") return makeToken(TokenType::WINDOW, value);
        // OpenGL functions
        if (value == "gl_init") return makeToken(TokenType::GL_INIT, value);
        if (value == "init_gl") return makeToken(TokenType::GL_INIT, value);
        if (value == "opengl") return makeToken(TokenType::GL_INIT, value);
        if (value == "set_clear_color") return makeToken(TokenType::SET_CLEAR_COLOR, value);
        if (value == "bg_color") return makeToken(TokenType::SET_CLEAR_COLOR, value);
        if (value == "draw_triangle") return makeToken(TokenType::DRAW_TRIANGLE, value);
        if (value == "triangle") return makeToken(TokenType::DRAW_TRIANGLE, value);
        if (value == "draw_quad") return makeToken(TokenType::DRAW_QUAD, value);
        if (value == "quad") return makeToken(TokenType::DRAW_QUAD, value);
        if (value == "fill_rect") return makeToken(TokenType::FILL_RECT, value);
        if (value == "box") return makeToken(TokenType::FILL_RECT, value);
        if (value == "draw_circle") return makeToken(TokenType::DRAW_CIRCLE, value);
        if (value == "circle") return makeToken(TokenType::DRAW_CIRCLE, value);
        if (value == "fill_circle") return makeToken(TokenType::FILL_CIRCLE, value);
        if (value == "ball") return makeToken(TokenType::FILL_CIRCLE, value);
        if (value == "set_projection_2d") return makeToken(TokenType::SET_PROJECTION_2D, value);
        if (value == "2d") return makeToken(TokenType::SET_PROJECTION_2D, value);
        if (value == "set_projection_3d") return makeToken(TokenType::SET_PROJECTION_3D, value);
        if (value == "3d") return makeToken(TokenType::SET_PROJECTION_3D, value);
        if (value == "translate") return makeToken(TokenType::TRANSLATE, value);
        if (value == "move") return makeToken(TokenType::TRANSLATE, value);
        if (value == "rotate") return makeToken(TokenType::ROTATE, value);
        if (value == "turn") return makeToken(TokenType::ROTATE, value);
        if (value == "scale") return makeToken(TokenType::SCALE, value);
        if (value == "size") return makeToken(TokenType::SCALE, value);
        if (value == "push_matrix") return makeToken(TokenType::PUSH_MATRIX, value);
        if (value == "push") return makeToken(TokenType::PUSH_MATRIX, value);
        if (value == "pop_matrix") return makeToken(TokenType::POP_MATRIX, value);
        if (value == "pop") return makeToken(TokenType::POP_MATRIX, value);
        if (value == "window_should_close") return makeToken(TokenType::WINDOW_SHOULD_CLOSE, value);
        if (value == "should_close") return makeToken(TokenType::WINDOW_SHOULD_CLOSE, value);
        if (value == "closed") return makeToken(TokenType::WINDOW_SHOULD_CLOSE, value);
        // Type keywords
        if (value == "int") return makeToken(TokenType::TYPE_INT, value);
        if (value == "number") return makeToken(TokenType::TYPE_INT, value);
        if (value == "num") return makeToken(TokenType::TYPE_INT, value);
        if (value == "str") return makeToken(TokenType::TYPE_STR, value);
        if (value == "string") return makeToken(TokenType::TYPE_STR, value);
        if (value == "text") return makeToken(TokenType::TYPE_STR, value);
        if (value == "float") return makeToken(TokenType::TYPE_FLOAT, value);
        if (value == "decimal") return makeToken(TokenType::TYPE_FLOAT, value);
        if (value == "real") return makeToken(TokenType::TYPE_FLOAT, value);
        if (value == "bool") return makeToken(TokenType::TYPE_BOOL, value);
        if (value == "boolean") return makeToken(TokenType::TYPE_BOOL, value);
        if (value == "flag") return makeToken(TokenType::TYPE_BOOL, value);
        if (value == "var") return makeToken(TokenType::TYPE_VAR, value);
        if (value == "auto") return makeToken(TokenType::TYPE_VAR, value);
        if (value == "any") return makeToken(TokenType::TYPE_VAR, value);
        // Format specifiers for output
        if (value == "bin" || value == "0b") return makeToken(TokenType::FMT_BIN, value);
        if (value == "hex" || value == "0x") return makeToken(TokenType::FMT_HEX, value);
        if (value == "oct" || value == "0o") return makeToken(TokenType::FMT_OCT, value);
        if (value == "dec" || value == "0d") return makeToken(TokenType::FMT_DEC, value);
        
        return makeToken(TokenType::IDENT, value);
    }

public:
    Lexer(const std::string& src) : source(src) {
        splitLines();
    }

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (current < source.length()) {
            skipWhitespace();
            if (current >= source.length()) break;
            
            skipComment();
            if (current >= source.length()) break;
            skipWhitespace();
            if (current >= source.length()) break;

            char c = peek();
            
            if (c == '"') {
                tokens.push_back(scanString());
            } else if (std::isdigit(c)) {
                tokens.push_back(scanNumber());
            } else if (std::isalpha(c) || c == '_') {
                tokens.push_back(scanIdentifier());
            } else if (c == '+') {
                advance();
                tokens.push_back(makeToken(TokenType::PLUS, "+"));
            } else if (c == '-') {
                advance();
                if (peek() == '>') {
                    advance();
                    tokens.push_back(makeToken(TokenType::ARROW, "->"));
                } else {
                    tokens.push_back(makeToken(TokenType::MINUS, "-"));
                }
            } else if (c == '*') {
                advance();
                tokens.push_back(makeToken(TokenType::STAR, "*"));
            } else if (c == '/') {
                advance();
                if (peek() == '/' || peek() == '*') {
                    current--;
                    column--;
                    skipComment();
                } else {
                    tokens.push_back(makeToken(TokenType::SLASH, "/"));
                }
            } else if (c == '%') {
                advance();
                tokens.push_back(makeToken(TokenType::PERCENT, "%"));
            } else if (c == '=') {
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::EQEQ, "=="));
                } else {
                    tokens.push_back(makeToken(TokenType::EQ, "="));
                }
            } else if (c == '!') {
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::NEQ, "!="));
                } else {
                    tokens.push_back(makeToken(TokenType::NOT, "!"));
                }
            } else if (c == '<') {
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LE, "<="));
                } else if (peek() == '<') {
                    advance();
                    tokens.push_back(makeToken(TokenType::SHL, "<<"));
                } else {
                    tokens.push_back(makeToken(TokenType::LT, "<"));
                }
            } else if (c == '>') {
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::GE, ">="));
                } else if (peek() == '>') {
                    advance();
                    tokens.push_back(makeToken(TokenType::SHR, ">>"));
                } else {
                    tokens.push_back(makeToken(TokenType::GT, ">"));
                }
            } else if (c == '&') {
                advance();
                if (peek() == '&') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LAND, "&&"));
                } else {
                    addError("Unexpected character '&'. Did you mean '&&'?");
                    tokens.push_back(makeToken(TokenType::UNKNOWN, "&"));
                }
            } else if (c == '|') {
                advance();
                if (peek() == '|') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LOR, "||"));
                } else {
                    addError("Unexpected character '|'. Did you mean '||'?");
                    tokens.push_back(makeToken(TokenType::UNKNOWN, "|"));
                }
            } else if (c == '(') {
                advance();
                tokens.push_back(makeToken(TokenType::LPAREN, "("));
            } else if (c == ')') {
                advance();
                tokens.push_back(makeToken(TokenType::RPAREN, ")"));
            } else if (c == '{') {
                advance();
                tokens.push_back(makeToken(TokenType::LBRACE, "{"));
            } else if (c == '}') {
                advance();
                tokens.push_back(makeToken(TokenType::RBRACE, "}"));
            } else if (c == '[') {
                advance();
                tokens.push_back(makeToken(TokenType::LBRACKET, "["));
            } else if (c == ']') {
                advance();
                tokens.push_back(makeToken(TokenType::RBRACKET, "]"));
            } else if (c == ';') {
                advance();
                tokens.push_back(makeToken(TokenType::SEMICOLON, ";"));
            } else if (c == ',') {
                advance();
                tokens.push_back(makeToken(TokenType::COMMA, ","));
            } else if (c == '.') {
                advance();
                tokens.push_back(makeToken(TokenType::DOT, "."));
            } else if (c == ':') {
                advance();
                tokens.push_back(makeToken(TokenType::COLON, ":"));
            } else {
                addError("Unexpected character: '" + std::string(1, c) + "' (ASCII " + std::to_string((int)c) + ")");
                advance();
                tokens.push_back(makeToken(TokenType::UNKNOWN, std::string(1, c)));
            }
        }
        
        tokens.push_back(makeToken(TokenType::END_OF_FILE, ""));
        return tokens;
    }

    bool hasErrors() const { return !errors.empty(); }
    
    const std::vector<LexerError>& getErrors() const { return errors; }
    
    void printErrors(std::ostream& out = std::cerr) const {
        for (const auto& err : errors) {
            out << "\033[1;31mLexer Error\033[0m at line " << err.line << ", column " << err.column << ":\n";
            out << "  " << err.message << "\n";
            if (!err.context.empty()) {
                out << "  " << err.context << "\n";
                out << "  ";
                for (int i = 1; i < err.column && i <= (int)err.context.length(); i++) {
                    out << (err.context[i-1] == '\t' ? '\t' : ' ');
                }
                out << "\033[1;31m^\033[0m\n";
            }
        }
    }
};

} // namespace disp

#endif // DISPLEXITY_LEXER_H
