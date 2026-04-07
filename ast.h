#pragma once
#include <string>
#include <memory>
#include <unordered_map>

// Базовый класс выражения
struct Expr {
    virtual ~Expr() = default;
    virtual double eval(const std::unordered_map<std::string, double>& vars) const = 0;
    virtual std::shared_ptr<Expr> diff(const std::string& var) const = 0;
    virtual std::string toString() const = 0;
};

// Число
struct NumberExpr : Expr {
    double val;
    explicit NumberExpr(double v);
    double eval(const std::unordered_map<std::string, double>& vars) const override;
    std::shared_ptr<Expr> diff(const std::string& var) const override;
    std::string toString() const override;
};

// Переменная
struct VariableExpr : Expr {
    std::string name;
    explicit VariableExpr(const std::string& n);
    double eval(const std::unordered_map<std::string, double>& vars) const override;
    std::shared_ptr<Expr> diff(const std::string& var) const override;
    std::string toString() const override;
};

// Бинарная операция
struct BinaryExpr : Expr {
    char op;
    std::shared_ptr<Expr> left, right;
    BinaryExpr(std::shared_ptr<Expr> l, char o, std::shared_ptr<Expr> r);
    double eval(const std::unordered_map<std::string, double>& vars) const override;
    std::shared_ptr<Expr> diff(const std::string& var) const override;
    std::string toString() const override;
};

// Унарная операция
struct UnaryExpr : Expr {
    char op;
    std::shared_ptr<Expr> operand;
    UnaryExpr(char o, std::shared_ptr<Expr> opnd);
    double eval(const std::unordered_map<std::string, double>& vars) const override;
    std::shared_ptr<Expr> diff(const std::string& var) const override;
    std::string toString() const override;
};

//FORWARD DECLARATION
struct FunctionExpr;  // для BinaryExpr::diff()

// Функция
struct FunctionExpr : Expr {
    std::string name;
    std::shared_ptr<Expr> arg;
    FunctionExpr(const std::string& n, std::shared_ptr<Expr> a);
    double eval(const std::unordered_map<std::string, double>& vars) const override;
    std::shared_ptr<Expr> diff(const std::string& var) const override;
    std::string toString() const override;
};
