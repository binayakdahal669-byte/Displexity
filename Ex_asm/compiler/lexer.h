#ifndef DISPLEXITY_LEXER_H
#define DISPLEXITY_LEXER_H

#include <string>
#include <vector>
#include <cctype>
#include <iostream>

namespace disp {

enum class TokenType {
    // Literals
    INT, STRING, TRUE, FALSE,
    // Keywords
    PRINT, IF, ELSE, WHILE, FOR, FUNC, CLASS, RETURN, INCLUDE, ARRAY,
    // Identifiers and operators
    IDENT, PLUS, MINUS, STAR, SLASH, PERCENT, EQ, EQEQ, NEQ, LT, LE, GT, GE, LAND, LOR, NOT,
    // Punctuation
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, SEMICOLON, COMMA, DOT, COLON, ARROW,
    // Special
    END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};

class Lexer {
private:
    std::string source;
    size_t current = 0;
    int line = 1;
    int column = 1;

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
    }

    Token makeToken(TokenType type, const std::string& lexeme) {
        return {type, lexeme, line, column};
    }

    Token scanString() {
        advance(); // opening quote
        std::string value;
        while (peek() != '"' && peek() != '\0') {
            if (peek() == '\\') {
                advance();
                switch (peek()) {
                    case 'n': value += '\n'; advance(); break;
                    case 't': value += '\t'; advance(); break;
                    case 'r': value += '\r'; advance(); break;
                    case '\\': value += '\\'; advance(); break;
                    case '"': value += '"'; advance(); break;
                    default: value += peek(); advance();
                }
            } else {
                value += peek();
                advance();
            }
        }
        if (peek() == '"') advance(); // closing quote
        return makeToken(TokenType::STRING, value);
    }

    Token scanNumber() {
        std::string value;
        while (std::isdigit(peek())) {
            value += peek();
            advance();
        }
        return makeToken(TokenType::INT, value);
    }

    Token scanIdentifier() {
        std::string value;
        while (std::isalnum(peek()) || peek() == '_') {
            value += peek();
            advance();
        }
        
        // Keywords
        if (value == "print") return makeToken(TokenType::PRINT, value);
        if (value == "if") return makeToken(TokenType::IF, value);
        if (value == "else") return makeToken(TokenType::ELSE, value);
        if (value == "while") return makeToken(TokenType::WHILE, value);
        if (value == "for") return makeToken(TokenType::FOR, value);
        if (value == "func") return makeToken(TokenType::FUNC, value);
        if (value == "class") return makeToken(TokenType::CLASS, value);
        if (value == "return") return makeToken(TokenType::RETURN, value);
        if (value == "include") return makeToken(TokenType::INCLUDE, value);
        if (value == "true") return makeToken(TokenType::TRUE, value);
        if (value == "false") return makeToken(TokenType::FALSE, value);
        if (value == "Array") return makeToken(TokenType::ARRAY, value);
        
        return makeToken(TokenType::IDENT, value);
    }

public:
    Lexer(const std::string& src) : source(src) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (current < source.length()) {
            skipWhitespace();
            if (current >= source.length()) break;
            
            skipComment();
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
                tokens.push_back(makeToken(TokenType::SLASH, "/"));
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
                } else {
                    tokens.push_back(makeToken(TokenType::LT, "<"));
                }
            } else if (c == '>') {
                advance();
                if (peek() == '=') {
                    advance();
                    tokens.push_back(makeToken(TokenType::GE, ">="));
                } else {
                    tokens.push_back(makeToken(TokenType::GT, ">"));
                }
            } else if (c == '&') {
                advance();
                if (peek() == '&') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LAND, "&&"));
                }
            } else if (c == '|') {
                advance();
                if (peek() == '|') {
                    advance();
                    tokens.push_back(makeToken(TokenType::LOR, "||"));
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
                advance();
                tokens.push_back(makeToken(TokenType::UNKNOWN, std::string(1, c)));
            }
        }
        
        tokens.push_back(makeToken(TokenType::END_OF_FILE, ""));
        return tokens;
    }
};

} // namespace disp

#endif // DISPLEXITY_LEXER_H
