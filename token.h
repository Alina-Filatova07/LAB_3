#pragma once
#include <string>

enum class TokenType {
    NUMBER, IDENTIFIER, PLUS, MINUS, MUL, DIV, POW, LPAREN, RPAREN, EOEX
};

struct Token {
    TokenType type;
    std::string value;
    
    Token(TokenType t = TokenType::EOEX, const std::string& v = "") 
        : type(t), value(v) {}
};
