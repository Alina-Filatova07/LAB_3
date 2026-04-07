#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "utils.h"

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::string command;
    if (!(std::cin >> command)) return 0;

    int n;
    std::cin >> n;

    std::vector<std::string> var_names(n);
    for (int i = 0; i < n; ++i) std::cin >> var_names[i];

    std::unordered_map<std::string, double> vars;
    for (int i = 0; i < n; ++i) {
        double val;
        std::cin >> val;
        std::string name = var_names[i];
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        vars[name] = val;
    }

    std::string expression;
    std::getline(std::cin >> std::ws, expression);

    try {
        if (command == "evaluate" || command == "evaluate_derivative") {
            Lexer lexer(expression);
            Parser parser(lexer);
            auto ast = parser.parse();
            
            if (command == "evaluate_derivative" && n > 0) {
                std::string diff_var = var_names[0];
                std::transform(diff_var.begin(), diff_var.end(), diff_var.begin(), ::tolower);
                auto deriv = ast->diff(diff_var);
                double result = deriv->eval(vars);
                printResult(result);
            } else if (command == "evaluate") {
                double result = ast->eval(vars);
                printResult(result);
            } else {
                throw std::runtime_error("No variable specified for derivative");
            }
        } else if (command == "derivative") {
            Lexer lexer(expression);
            Parser parser(lexer);
            auto ast = parser.parse();
            
            if (n > 0) {
                std::string diff_var = var_names[0];
                std::transform(diff_var.begin(), diff_var.end(), diff_var.begin(), ::tolower);
                auto deriv = ast->diff(diff_var);
                std::cout << deriv->toString() << "\n";
            } else {
                throw std::runtime_error("No variable specified for derivative");
            }
        } else {
            throw std::runtime_error("Unknown command: " + command);
        }
    } catch (const std::exception& e) {
        std::cout << "ERROR " << e.what() << "\n";
    } catch (...) {
        std::cout << "ERROR Unknown exception\n";
    }

    return 0;
}

