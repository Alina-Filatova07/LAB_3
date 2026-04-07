#pragma once
#include "lexer.h"
#include "ast.h"
#include <memory>

class Parser {
private:
    Lexer& lexer;
    Token current;
    
    void next();
    std::shared_ptr<Expr> parseExpression();
    std::shared_ptr<Expr> parseTerm();
    std::shared_ptr<Expr> parseUnary();
    std::shared_ptr<Expr> parsePower();
    std::shared_ptr<Expr> parsePrimary();
    
public:
    explicit Parser(Lexer& lex);
    std::shared_ptr<Expr> parse();
};

