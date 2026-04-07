#include "parser.h"
#include <stdexcept>

void Parser::next() { current = lexer.next(); }

std::shared_ptr<Expr> Parser::parseExpression() {
    auto node = parseTerm();
    while (current.type == TokenType::PLUS || current.type == TokenType::MINUS) {
        char op = current.value[0];
        next();
        node = std::make_shared<BinaryExpr>(std::move(node), op, parseTerm());
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseTerm() {
    auto node = parseUnary();
    while (current.type == TokenType::MUL || current.type == TokenType::DIV) {
        char op = current.value[0];
        next();
        node = std::make_shared<BinaryExpr>(std::move(node), op, parseUnary());
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseUnary() {
    if (current.type == TokenType::PLUS || current.type == TokenType::MINUS) {
        char op = current.value[0];
        next();
        return std::make_shared<UnaryExpr>(op, parseUnary());
    }
    return parsePower();
}

std::shared_ptr<Expr> Parser::parsePower() {
    auto left = parsePrimary();
    if (current.type == TokenType::POW) {
        next();
        auto right = parseUnary();
        return std::make_shared<BinaryExpr>(std::move(left), '^', std::move(right));
    }
    return left;
}

std::shared_ptr<Expr> Parser::parsePrimary() {
    if (current.type == TokenType::NUMBER) {
        std::string val = current.value;
        next();
        return std::make_shared<NumberExpr>(std::stod(val));
    }
    if (current.type == TokenType::IDENTIFIER) {
        std::string id = current.value;
        next();
        if (current.type == TokenType::LPAREN) {
            next();
            auto arg = parseExpression();
            if (current.type != TokenType::RPAREN) throw std::runtime_error("Expected ')'");
            next();
            return std::make_shared<FunctionExpr>(std::move(id), std::move(arg));
        }
        return std::make_shared<VariableExpr>(std::move(id));
    }
    if (current.type == TokenType::LPAREN) {
        next();
        auto expr = parseExpression();
        if (current.type != TokenType::RPAREN) throw std::runtime_error("Expected ')'");
        next();
        return expr;
    }
    throw std::runtime_error("Unexpected token: " + current.value);
}

Parser::Parser(Lexer& lex) : lexer(lex) { next(); }

std::shared_ptr<Expr> Parser::parse() {
    auto expr = parseExpression();
    if (current.type != TokenType::EOEX)
        throw std::runtime_error("Unexpected tokens after expression");
    return expr;
}
