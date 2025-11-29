#pragma once

#include <memory>
#include <string>

#include "Expression.hpp"

class Program;
class VarState;

class Statement {
 public:
  explicit Statement(std::string source);
  virtual ~Statement() = default;

  virtual void execute(VarState& state, Program& program) const = 0;

  const std::string& text() const noexcept;

 private:
  std::string source_;
};

// TODO: Other statement types derived from Statement, e.g., GOTOStatement,
// LetStatement, etc.
// LET变量赋值
class LetStatement : public Statement {
 public:
  LetStatement(std::string source, std::string var_name, Expression* expr)
      : Statement(std::move(source)), var_name_(std::move(var_name)), assignexpr(expr) {}

  ~LetStatement() override { delete assignexpr; }

  void execute(VarState& state, Program& program) const override;

 private:
  std::string var_name_;  // 变量名
  Expression* assignexpr;  // 赋值表达式
};

// PRINT输出
class PrintStatement : public Statement {
 public:
  PrintStatement(std::string source, Expression* expr)
      : Statement(std::move(source)), assignexpr(expr) {}

  ~PrintStatement() override { delete assignexpr; }

  void execute(VarState& state, Program& program) const override;

 private:
  Expression* assignexpr;  // 要输出的表达式
};

// INPUT输入
class InputStatement : public Statement {
 public:
  InputStatement(std::string source, std::string var_name)
      : Statement(std::move(source)), var_name_(std::move(var_name)) {}

  void execute(VarState& state, Program& program) const override;

 private:
  std::string var_name_;  // 要输入的变量名
};

// GOTO跳转
class GotoStatement : public Statement {
 public:
  GotoStatement(std::string source, int target_line)
      : Statement(std::move(source)), target_line_(target_line) {}

  void execute(VarState& state, Program& program) const override;

 private:
  int target_line_;  // 目标行号
};

// IF条件跳转
class IfStatement : public Statement {
 public:
  IfStatement(std::string source, Expression* left, char op, Expression* right, int target_line)
      : Statement(std::move(source)), left_(left), op_(op), right_(right), target_line_(target_line) {}

  ~IfStatement() override {
    delete left_;
    delete right_;
  }

  void execute(VarState& state, Program& program) const override;

 private:
  Expression* left_;      // 左表达式
  char op_;               // 运算符
  Expression* right_;     // 右表达式
  int target_line_;       // 跳转行号
};

// REM注释
class RemStatement : public Statement {
 public:
  explicit RemStatement(std::string source) : Statement(std::move(source)) {}

  void execute(VarState& state, Program& program) const override;
};

// END终止
class EndStatement : public Statement {
 public:
  explicit EndStatement(std::string source) : Statement(std::move(source)) {}

  void execute(VarState& state, Program& program) const override;
};