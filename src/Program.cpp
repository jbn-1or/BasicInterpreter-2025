// TODO: Imply interfaces declared in the Program.hpp.
#include "Program.hpp"

#include <iostream>
#include "Recorder.hpp"
#include "Statement.hpp"
#include "VarState.hpp"

Program::Program() : programCounter_(0), programEnd_(false) {}

void Program::addStmt(int line, Statement* stmt) {
    removeStmt(line);
    recorder_.add(line, stmt);
}

void Program::removeStmt(int line) {
    recorder_.remove(line);
}

// 修改 src/Program.cpp 中的 run() 方法
void Program::run() {
    resetAfterRun(); // 重置运行状态（PC、结束标志）
    
    // 获取最小行号（第一个行号）
    int firstLine = recorder_.nextLine(0);
    if (firstLine == -1) {
        return; // 无程序行时直接返回
    }
    
    // 从开头运行
    programCounter_ = firstLine;
    programEnd_ = false;

    while (!programEnd_) {
        const Statement* currentStmt = recorder_.get(programCounter_);
        if (!currentStmt) {
            // 若当前行号不存在有效语句，终止运行
            programEnd_ = true;
            break;
        }

        // 记录当前PC用于判断是否被修改（如GOTO/IF）
        int currentPC = programCounter_;

        // 执行当前语句
        currentStmt->execute(vars_, *this);

        if (programEnd_) {
            break;
        }

        // 检查PC是否被跳转语句修改
        if (programCounter_ != currentPC) {
            // 如果PC被修改，直接进入下一次循环执行目标行
            continue;
        }

        // PC未被修改，自动进入下一行
        int nextLine = recorder_.nextLine(currentPC);
        if (nextLine == -1) {
            // 无下一行时终止
            programEnd_ = true;
        } else {
            programCounter_ = nextLine;
        }
    }

    resetAfterRun();
}

void Program::list() const {
    int currentLine = 0;
    while (true) {
        int nextLine = recorder_.nextLine(currentLine);
        if (nextLine == -1) {
            break;
        }
        const Statement* stmt = recorder_.get(nextLine);
        if (stmt) {
            // 提取 stmt->text() 中除去行号前缀的纯语句内容
            std::string text = stmt->text();
            // 找到第一个非数字字符的位置（跳过行号）
            size_t firstNonDigit = text.find_first_not_of("0123456789");
            if (firstNonDigit != std::string::npos) {
                // 跳过行号后的空格
                size_t firstNonSpace = text.find_first_not_of(" \t", firstNonDigit);
                if (firstNonSpace != std::string::npos) {
                    text = text.substr(firstNonSpace); // 截取纯语句内容
                } else {
                    text = ""; // 若行号后只有空格，则视为空语句
                }
            }
            // 输出：行号 + 空格 + 纯语句内容
            std::cout << nextLine << " " << text << std::endl;
        }
        currentLine = nextLine;
    }
}

void Program::clear() {
    recorder_.clear();
    vars_.clear();
    resetAfterRun();
}

void Program::execute(Statement* stmt) {
    stmt->execute(vars_, *this);
}

int Program::getPC() const noexcept {
    return programCounter_;
}

void Program::changePC(int line) {
    programCounter_ = line;
}

void Program::programEnd() {
    programEnd_ = true;
}

void Program::resetAfterRun() noexcept {
    programCounter_ = 0;
    programEnd_ = false;
}