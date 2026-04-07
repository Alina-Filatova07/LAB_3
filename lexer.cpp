#include "lexer.h"
#include <cctype>
#include <algorithm>
#include <stdexcept>

void Lexer::skipWhitespace() {
    while (pos < expr.size() && std::isspace(static_cast<unsigned char>(expr[pos]))) ++pos;
}

bool Lexer::isDigit(char c) const { return std::isdigit(static_cast<unsigned char>(c)); }
bool Lexer::isLetter(char c) const { return std::isalpha(static_cast<unsigned char>(c)); }
bool Lexer::isLetterOrDigit(char c) const { return std::isalnum(static_cast<unsigned char>(c)); }

Token Lexer::readNumber() {
    std::string num;
    if (expr[pos] == '0') {
        num += expr[pos++];
        if (pos < expr.size() && isDigit(expr[pos]))
            throw std::runtime_error("Leading zeros not allowed");
    } else if (isDigit(expr[pos])) {
        while (pos < expr.size() && isDigit(expr[pos])) num += expr[pos++];
    } else {
        throw std::runtime_error("Invalid number");
    }

    if (pos < expr.size() && expr[pos] == '.') {
        if (pos + 1 >= expr.size() || !isDigit(expr[pos + 1]))
            throw std::runtime_error("Expected digit after dot");
        num += expr[pos++];
        while (pos < expr.size() && isDigit(expr[pos])) num += expr[pos++];
    }

    if (pos < expr.size() && (expr[pos] == 'e' || expr[pos] == 'E')) {
        num += expr[pos++];
        if (pos < expr.size() && (expr[pos] == '+' || expr[pos] == '-')) num += expr[pos++];
        if (pos >= expr.size() || !isDigit(expr[pos]))
            throw std::runtime_error("Invalid exponent");
        while (pos < expr.size() && isDigit(expr[pos])) num += expr[pos++];
    }

    if (pos < expr.size() && isLetterOrDigit(expr[pos]))
        throw std::runtime_error("Invalid character after number");

    return Token(TokenType::NUMBER, num);
}

Token Lexer::readIdentifier() {
    std::string id;
    while (pos < expr.size() && (isLetterOrDigit(expr[pos]) || expr[pos] == '_'))
        id += expr[pos++];
    if (id.empty()) throw std::runtime_error("Invalid identifier");
    std::transform(id.begin(), id.end(), id.begin(), ::tolower);
    return Token(TokenType::IDENTIFIER, id);
}

Lexer::Lexer(const std::string& expression) : expr(expression), pos(0) {}

Token Lexer::next() {
    skipWhitespace();
    if (pos >= expr.size()) return Token(TokenType::EOEX);

    char c = expr[pos];
    switch (c) {
        case '+': ++pos; return Token(TokenType::PLUS, "+");
        case '-': ++pos; return Token(TokenType::MINUS, "-");
        case '*': ++pos; return Token(TokenType::MUL, "*");
        case '/': ++pos; return Token(TokenType::DIV, "/");
        case '^': ++pos; return Token(TokenType::POW, "^");
        case '(': ++pos; return Token(TokenType::LPAREN, "(");
        case ')': ++pos; return Token(TokenType::RPAREN, ")");
    }

    if (isDigit(c)) return readNumber();
    if (c == '.') throw std::runtime_error("Number cannot start with dot");
    if (isLetter(c) || c == '_') return readIdentifier();

    throw std::runtime_error("Unknown symbol");
}
