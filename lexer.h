#pragma once
#include <string>

enum class TokenType {
    NUMBER, IDENTIFIER, PLUS, MINUS, MUL, DIV, POW, LPAREN, RPAREN, EOEX
};

class Token {
public:
    TokenType type;
    std::string value;
    
    Token(TokenType t = TokenType::EOEX, const std::string& v = "") 
        : type(t), value(v) {}
    
    std::string view() const { return value; }
};

class Lexer {
private:
    std::string expr;
    size_t pos;
    
    void skipWhitespace();
    bool isDigit(char c) const;
    bool isLetter(char c) const;
    bool isLetterOrDigit(char c) const;
    
    Token readNumber();
    Token readIdentifier();
    
public:
    explicit Lexer(const std::string& expression);
    Token next();
};
