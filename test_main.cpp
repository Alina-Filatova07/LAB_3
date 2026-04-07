#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "test_helpers.h"

int main() {
    std::cout << "\nLAB 3 TESTS" << std::endl;
    
    // LEXER TESTS
    std::cout << "\n[LEXER TESTS]" << std::endl;
    
    run_test("Simple number", []() {
        Lexer lexer("123");
        Token tok = lexer.next();
        REQUIRE(tok.type == TokenType::NUMBER);
        REQUIRE(tok.value == "123");
    });
    
    run_test("Simple identifier", []() {
        Lexer lexer("x");
        Token tok = lexer.next();
        REQUIRE(tok.type == TokenType::IDENTIFIER);
        REQUIRE(tok.value == "x");
    });
    
    run_test("Simple expression", []() {
        Lexer lexer("1 + 2");
        Token t1 = lexer.next();
        Token t2 = lexer.next();
        Token t3 = lexer.next();
        REQUIRE(t1.value == "1");
        REQUIRE(t2.value == "+");
        REQUIRE(t3.value == "2");
    });
    
    run_test("Complex expression", []() {
        Lexer lexer("-(1 + 2) * 3 ^ 2");
        std::vector<std::string> expected = {"-", "(", "1", "+", "2", ")", "*", "3", "^", "2"};
        for (const auto& exp : expected) {
            Token tok = lexer.next();
            REQUIRE(tok.value == exp);
        }
    });
    
    run_test("Function call", []() {
        Lexer lexer("sin(3.14)");
        Token t1 = lexer.next();
        Token t2 = lexer.next();
        Token t3 = lexer.next();
        Token t4 = lexer.next();
        REQUIRE(t1.value == "sin");
        REQUIRE(t2.value == "(");
        REQUIRE(t3.value == "3.14");
        REQUIRE(t4.value == ")");
    });
    
    // PARSER TESTS
    std::cout << "\n[PARSER TESTS]" << std::endl;
    
    run_test("Simple addition", []() {
        Lexer lexer("1 + 2");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "(+ 1 2)");
    });
    
    run_test("Simple multiplication", []() {
        Lexer lexer("3 * 4");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "(* 3 4)");
    });
    
    run_test("Operator precedence", []() {
        Lexer lexer("1 + 2 * 3");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "(+ 1 (* 2 3))");
    });
    
    run_test("Parentheses", []() {
        Lexer lexer("(1 + 2) * 3");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "(* (+ 1 2) 3)");
    });
    
    run_test("Complex expression", []() {
        Lexer lexer("-(1 + 2) * 3 ^ 2");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "(* (- (+ 1 2)) (^ 3 2))");
    });
    
    run_test("Function call", []() {
        Lexer lexer("sin(3.14)");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "sin(3.14)");
    });
    
    run_test("Nested functions", []() {
        Lexer lexer("sin(cos(x))");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "sin(cos(x))");
    });
    
    run_test("Power right associativity", []() {
        Lexer lexer("2 ^ 3 ^ 2");
        Parser parser(lexer);
        auto ast = parser.parse();
        REQUIRE(ast->toString() == "(^ 2 (^ 3 2))");
    });
    
    // EVALUATION TESTS
    std::cout << "\n[EVALUATION TESTS]" << std::endl;
    
    run_test("Evaluate simple", []() {
        Lexer lexer("2 + 2");
        Parser parser(lexer);
        auto ast = parser.parse();
        ASSERT_NEAR(ast->eval({}), 4.0, 1e-6);
    });
    
    run_test("Evaluate with variables", []() {
        Lexer lexer("x + y");
        Parser parser(lexer);
        auto ast = parser.parse();
        ASSERT_NEAR(ast->eval({{"x", 3}, {"y", 4}}), 7.0, 1e-6);
    });
    
    run_test("Evaluate power", []() {
        Lexer lexer("2 ^ 3");
        Parser parser(lexer);
        auto ast = parser.parse();
        ASSERT_NEAR(ast->eval({}), 8.0, 1e-6);
    });
    
    run_test("Evaluate function", []() {
        Lexer lexer("sin(0)");
        Parser parser(lexer);
        auto ast = parser.parse();
        ASSERT_NEAR(ast->eval({}), 0.0, 1e-6);
    });
    
    run_test("Evaluate complex", []() {
        Lexer lexer("x * x + y * 2");
        Parser parser(lexer);
        auto ast = parser.parse();
        ASSERT_NEAR(ast->eval({{"x", 2}, {"y", 3}}), 10.0, 1e-6);
    });
    
    // DERIVATIVE TESTS
    std::cout << "\n[DERIVATIVE TESTS]" << std::endl;
    
    run_test("Derivative of constant", []() {
        Lexer lexer("5");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        ASSERT_NEAR(deriv->eval({}), 0.0, 1e-6);
    });
    
    run_test("Derivative of x", []() {
        Lexer lexer("x");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        ASSERT_NEAR(deriv->eval({}), 1.0, 1e-6);
    });
    
    run_test("Derivative of x^2", []() {
        Lexer lexer("x^2");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        ASSERT_NEAR(deriv->eval({{"x", 2}}), 4.0, 1e-6);
    });
    
    run_test("Derivative of sin(x)", []() {
        Lexer lexer("sin(x)");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        ASSERT_NEAR(deriv->eval({{"x", 0}}), 1.0, 1e-6);
    });
    
    run_test("Derivative of cos(x)", []() {
        Lexer lexer("cos(x)");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        double result = deriv->eval({{"x", 0}});
        ASSERT_NEAR(std::abs(result), 0.0, 1e-6);
    });
    
    run_test("Derivative of x*y by x", []() {
        Lexer lexer("x*y");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        ASSERT_NEAR(deriv->eval({{"x", 2}, {"y", 3}}), 3.0, 1e-6);
    });
    
    run_test("Derivative of x^3", []() {
        Lexer lexer("x^3");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        ASSERT_NEAR(deriv->eval({{"x", 3}}), 27.0, 1e-6);
    });
    
    run_test("Derivative of 1/x", []() {
        Lexer lexer("1/x");
        Parser parser(lexer);
        auto ast = parser.parse();
        auto deriv = ast->diff("x");
        ASSERT_NEAR(deriv->eval({{"x", 2}}), -0.25, 1e-6);
    });
    
    // SUMMARY
    std::cout << "\nTEST SUMMARY" << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    std::cout << "Total:  " << (tests_passed + tests_failed) << std::endl;
    
    if (tests_failed == 0) {
        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
    } else {
        std::cerr << "\nSome tests failed!" << std::endl;
        return 1;
    }
}
