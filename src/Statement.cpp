#include "Statement.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "Program.hpp"
#include "VarState.hpp"
#include "utils/Error.hpp"

Statement::Statement(std::string source) : source_(std::move(source)) {}

const std::string& Statement::text() const noexcept { return source_; }

// TODO: Imply interfaces declared in the Statement.hpp.

void LetStatement::execute(VarState& state, Program& program) const {
    int value = assignexpr->evaluate(state);
    state.setValue(var_name_, value);
}

void PrintStatement::execute(VarState& state, Program& program) const {
    int value = assignexpr->evaluate(state);
    std::cout << value << std::endl;
}

void InputStatement::execute(VarState& state, Program& program) const {
    std::cout << "?";
    int value;
    std::cin >> value;
    
    // 处理输入错误
    if (!std::cin) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw BasicError("INPUT ERROR");
    }
    
    state.setValue(var_name_, value);
}

void GotoStatement::execute(VarState& state, Program& program) const {
    program.changePC(target_line_);
}

void IfStatement::execute(VarState& state, Program& program) const {
    int lhs = left_->evaluate(state);
    int rhs = right_->evaluate(state);
    bool condition = false;
    
    switch (op_) {
        case '=':
            condition = (lhs == rhs);
            break;
        case '>':
            condition = (lhs > rhs);
            break;
        case '<':
            condition = (lhs < rhs);
            break;
        default:
            throw BasicError("INVALID OPERATOR");
    }
    
    if (condition) {
        program.changePC(target_line_);
    }
}

void RemStatement::execute(VarState& state, Program& program) const {
}

void EndStatement::execute(VarState& state, Program& program) const {
    program.programEnd();
}