#include "ast.h"
#include <cmath>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <limits>

//NumberExpr
NumberExpr::NumberExpr(double v) : val(v) {}

double NumberExpr::eval(const std::unordered_map<std::string, double>&) const {
    return val;
}

std::shared_ptr<Expr> NumberExpr::diff(const std::string&) const {
    return std::make_shared<NumberExpr>(0.0);
}

std::string NumberExpr::toString() const {
    std::ostringstream oss;
    oss << std::setprecision(15) << val;
    std::string s = oss.str();
    if (s.find('.') != std::string::npos) {
        s.erase(s.find_last_not_of('0') + 1, std::string::npos);
        if (s.back() == '.') s.pop_back();
    }
    return s;
}

//VariableExpr
VariableExpr::VariableExpr(const std::string& n) : name(n) {}

double VariableExpr::eval(const std::unordered_map<std::string, double>& vars) const {
    auto it = vars.find(name);
    if (it == vars.end()) throw std::runtime_error("Unknown variable: " + name);
    return it->second;
}

std::shared_ptr<Expr> VariableExpr::diff(const std::string& var) const {
    return std::make_shared<NumberExpr>(name == var ? 1.0 : 0.0);
}

std::string VariableExpr::toString() const {
    return name;
}

//BinaryExpr
BinaryExpr::BinaryExpr(std::shared_ptr<Expr> l, char o, std::shared_ptr<Expr> r) 
    : op(o), left(std::move(l)), right(std::move(r)) {}

double BinaryExpr::eval(const std::unordered_map<std::string, double>& vars) const {
    double l = left->eval(vars), r = right->eval(vars);
    switch (op) {
        case '+': return l + r;
        case '-': return l - r;
        case '*': return l * r;
        case '/': return l / r;
        case '^': {
            if (l < 0 && std::floor(r) != r) 
                throw std::runtime_error("Domain error: pow with negative base");
            return std::pow(l, r);
        }
    }
    throw std::runtime_error("Invalid operator");
}

std::shared_ptr<Expr> BinaryExpr::diff(const std::string& var) const {
    auto dl = left->diff(var);
    auto dr = right->diff(var);
    
    switch (op) {
        case '+':
        case '-': {
            char newOp = (op == '+') ? '+' : '-';
            return std::make_shared<BinaryExpr>(dl, newOp, dr);
        }
        case '*': {
            auto term1 = std::make_shared<BinaryExpr>(dl, '*', right);
            auto term2 = std::make_shared<BinaryExpr>(left, '*', dr);
            return std::make_shared<BinaryExpr>(term1, '+', term2);
        }
        case '/': {
            auto num1 = std::make_shared<BinaryExpr>(dl, '*', right);
            auto num2 = std::make_shared<BinaryExpr>(left, '*', dr);
            auto numerator = std::make_shared<BinaryExpr>(num1, '-', num2);
            auto denominator = std::make_shared<BinaryExpr>(right, '*', right);
            return std::make_shared<BinaryExpr>(numerator, '/', denominator);
        }
        case '^': {
            try {
                std::unordered_map<std::string, double> empty;
                double rv = right->eval(empty);
                if (!std::isnan(rv) && !std::isinf(rv)) {
                    auto c = std::make_shared<NumberExpr>(rv);
                    auto u_pow_c_minus_1 = std::make_shared<BinaryExpr>(left, '^', 
                        std::make_shared<BinaryExpr>(c, '-', std::make_shared<NumberExpr>(1.0)));
                    auto term = std::make_shared<BinaryExpr>(c, '*', u_pow_c_minus_1);
                    return std::make_shared<BinaryExpr>(term, '*', dl);
                }
            } catch (...) {}
            
            auto ln_u = std::make_shared<FunctionExpr>("log", left);
            auto term1 = std::make_shared<BinaryExpr>(dr, '*', ln_u);
            auto term2_inner = std::make_shared<BinaryExpr>(right, '*', dl);
            auto term2 = std::make_shared<BinaryExpr>(term2_inner, '/', left);
            auto sum = std::make_shared<BinaryExpr>(term1, '+', term2);
            auto u_pow_v = std::make_shared<BinaryExpr>(left, '^', right);
            return std::make_shared<BinaryExpr>(u_pow_v, '*', sum);
        }
    }
    throw std::runtime_error("Invalid operator in diff");
}

std::string BinaryExpr::toString() const {
    return "(" + left->toString() + " " + std::string(1, op) + " " + right->toString() + ")";
}

//UnaryExpr
UnaryExpr::UnaryExpr(char o, std::shared_ptr<Expr> opnd) : op(o), operand(std::move(opnd)) {}

double UnaryExpr::eval(const std::unordered_map<std::string, double>& vars) const {
    double v = operand->eval(vars);
    return op == '-' ? -v : v;
}

std::shared_ptr<Expr> UnaryExpr::diff(const std::string& var) const {
    auto d = operand->diff(var);
    if (op == '-') {
        return std::make_shared<UnaryExpr>('-', d);
    }
    return d;
}

std::string UnaryExpr::toString() const {
    if (op == '-') return "-" + operand->toString();
    return operand->toString();
}

//FunctionExpr
FunctionExpr::FunctionExpr(const std::string& n, std::shared_ptr<Expr> a) : name(n), arg(std::move(a)) {}

double FunctionExpr::eval(const std::unordered_map<std::string, double>& vars) const {
    double v = arg->eval(vars);
    if (name == "sin") return std::sin(v);
    if (name == "cos") return std::cos(v);
    if (name == "tan") {
        if (std::abs(std::cos(v)) < 1e-12) throw std::runtime_error("Domain error: tan");
        return std::tan(v);
    }
    if (name == "asin") {
        if (v < -1.0 || v > 1.0) throw std::runtime_error("Domain error: asin");
        return std::asin(v);
    }
    if (name == "acos") {
        if (v < -1.0 || v > 1.0) throw std::runtime_error("Domain error: acos");
        return std::acos(v);
    }
    if (name == "atan") return std::atan(v);
    if (name == "exp") return std::exp(v);
    if (name == "log") {
        if (v < 0.0) throw std::runtime_error("Domain error: log");
        if (v == 0.0) return -std::numeric_limits<double>::infinity();
        return std::log(v);
    }
    if (name == "sqrt") {
        if (v < 0.0) throw std::runtime_error("Domain error: sqrt");
        return std::sqrt(v);
    }
    throw std::runtime_error("Unknown function: " + name);
}

std::shared_ptr<Expr> FunctionExpr::diff(const std::string& var) const {
    auto d_arg = arg->diff(var);
    
    if (name == "sin") {
        auto cos_u = std::make_shared<FunctionExpr>("cos", arg);
        return std::make_shared<BinaryExpr>(cos_u, '*', d_arg);
    }
    if (name == "cos") {
        auto sin_u = std::make_shared<FunctionExpr>("sin", arg);
        auto neg_sin = std::make_shared<UnaryExpr>('-', sin_u);
        return std::make_shared<BinaryExpr>(neg_sin, '*', d_arg);
    }
    if (name == "tan") {
        auto cos_u = std::make_shared<FunctionExpr>("cos", arg);
        auto cos_sq = std::make_shared<BinaryExpr>(cos_u, '^', std::make_shared<NumberExpr>(2.0));
        auto sec_sq = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '/', cos_sq);
        return std::make_shared<BinaryExpr>(sec_sq, '*', d_arg);
    }
    if (name == "asin") {
        auto u_sq = std::make_shared<BinaryExpr>(arg, '^', std::make_shared<NumberExpr>(2.0));
        auto one_minus_u_sq = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '-', u_sq);
        auto sqrt_term = std::make_shared<FunctionExpr>("sqrt", one_minus_u_sq);
        auto deriv = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '/', sqrt_term);
        return std::make_shared<BinaryExpr>(deriv, '*', d_arg);
    }
    if (name == "acos") {
        auto u_sq = std::make_shared<BinaryExpr>(arg, '^', std::make_shared<NumberExpr>(2.0));
        auto one_minus_u_sq = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '-', u_sq);
        auto sqrt_term = std::make_shared<FunctionExpr>("sqrt", one_minus_u_sq);
        auto deriv = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '/', sqrt_term);
        auto neg_deriv = std::make_shared<UnaryExpr>('-', deriv);
        return std::make_shared<BinaryExpr>(neg_deriv, '*', d_arg);
    }
    if (name == "atan") {
        auto u_sq = std::make_shared<BinaryExpr>(arg, '^', std::make_shared<NumberExpr>(2.0));
        auto one_plus_u_sq = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '+', u_sq);
        auto deriv = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '/', one_plus_u_sq);
        return std::make_shared<BinaryExpr>(deriv, '*', d_arg);
    }
    if (name == "exp") {
        auto exp_u = std::make_shared<FunctionExpr>("exp", arg);
        return std::make_shared<BinaryExpr>(exp_u, '*', d_arg);
    }
    if (name == "log") {
        auto deriv = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '/', arg);
        return std::make_shared<BinaryExpr>(deriv, '*', d_arg);
    }
    if (name == "sqrt") {
        auto sqrt_u = std::make_shared<FunctionExpr>("sqrt", arg);
        auto two_sqrt = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(2.0), '*', sqrt_u);
        auto deriv = std::make_shared<BinaryExpr>(std::make_shared<NumberExpr>(1.0), '/', two_sqrt);
        return std::make_shared<BinaryExpr>(deriv, '*', d_arg);
    }
    throw std::runtime_error("Unknown function in diff: " + name);
}

std::string FunctionExpr::toString() const {
    return name + "(" + arg->toString() + ")";
}

