#include "Statement.hpp"

#include <iostream>
#include <limits>
#include <sstream>
#include <utility>

#include "Program.hpp"
#include "VarState.hpp"
#include "utils/Error.hpp"

bool isValidInteger(const std::string& s) {
    if (s.empty()) return false;
    size_t i = 0;

    // 跳过前导空格
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
        ++i;
    }
    if (i == s.size()) return false; // 全是空格的输入

    // 处理正负号（可选）
    if (s[i] == '+' || s[i] == '-') {
        ++i;
        if (i == s.size()) return false; // 只有符号的输入
    }

    // 剩余字符必须全是数字
    while (i < s.size()) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) {
            return false; // 包含非数字字符（如小数点）
        }
        ++i;
    }
    return true;
}

Statement::Statement(std::string source) : source_(std::move(source)) {}

const std::string& Statement::text() const noexcept { return source_; }

// TODO: Imply interfaces declared in the Statement.hpp.

void LetStatement::execute(VarState& state, Program& program) const {
    int value = assignexpr->evaluate(state);
    state.setValue(var_name_, value);
}

void PrintStatement::execute(VarState& state, Program& program) const {
    try {
        int value = assignexpr->evaluate(state);
        std::cout << value << std::endl;
    } catch (const BasicError& e) {
        // 捕获变量未定义的错误，输出指定信息
        if (e.what() == std::string("VARIABLE NOT DEFINED")) {
            std::cout << "VARIABLE NOT DEFINED" << std::endl;
        } else {
            throw; // 其他错误继续抛出
        }
    }
}

void InputStatement::execute(VarState& state, Program& program) const {
    int value;
    std::string input;

    while (true) {
        std::cout << " ? ";
        // 读取一整行输入（包含所有字符，包括空格和特殊符号）
        if (!std::getline(std::cin, input)) {
            // 处理输入流错误
            std::cin.clear();
            std::cout << "INVALID NUMBER" << std::endl;
            continue;
        }
        // 修剪输入前后的空格
        size_t start = input.find_first_not_of(" \t");
        if (start == std::string::npos) {
            // 空输入（全是空格）
            std::cout << "INVALID NUMBER" << std::endl;
            continue;
        }
        size_t end = input.find_last_not_of(" \t");
        std::string trimmed = input.substr(start, end - start + 1);

        // 验证是否为有效整数（排除小数）
        if (isValidInteger(trimmed)) {
            try {
                // 检查整数范围是否在int可表示范围内
                long long num = std::stoll(trimmed);
                if (num < std::numeric_limits<int>::min() || num > std::numeric_limits<int>::max()) {
                    throw std::out_of_range("");
                }
                value = static_cast<int>(num);
                break; // 输入合法，退出循环
            } catch (const std::out_of_range&) {
                // 整数超出范围
                std::cout << "INVALID NUMBER" << std::endl;
            } catch (const std::invalid_argument&) {
                std::cout << "INVALID NUMBER" << std::endl;
            }
        } else {
            // 包含非数字字符（如小数点、字母等）
            std::cout << "INVALID NUMBER" << std::endl;
        }
    }
    state.setValue(var_name_, value);
}

void GotoStatement::execute(VarState& state, Program& program) const {
    if (!program.hasLine(target_line_)) {
        throw BasicError("LINE NUMBER ERROR");
    }
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
            throw BasicError("SYNTAX ERROR");
    }
    
    if (condition) {
        if (!program.hasLine(target_line_)) {
            throw BasicError("LINE NUMBER ERROR");
        }
        program.changePC(target_line_);
    }
}

void RemStatement::execute(VarState& state, Program& program) const {
}

void EndStatement::execute(VarState& state, Program& program) const {
    program.programEnd();
}