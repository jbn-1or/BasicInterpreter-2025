// TODO: Imply interfaces declared in the Program.hpp.
#include "Program.hpp"

#include <iostream>
#include "Recorder.hpp"
#include "Statement.hpp"
#include "VarState.hpp"

Program::Program() : programCounter_(0), programEnd_(false) {}

void Program::addStmt(int line, Statement* stmt) {
    recorder_.add(line, stmt);
}

void Program::removeStmt(int line) {
    recorder_.remove(line);
}

void Program::run() {
    resetAfterRun();
    // 获取第一个行号
    int firstLine = recorder_.nextLine(0);
    if (firstLine == -1) {
        return;
    }
    programCounter_ = firstLine;
    programEnd_ = false;

    while (!programEnd_) {
        const Statement* currentStmt = recorder_.get(programCounter_);
        if (!currentStmt) {
            // 不存在有效语句
            programEnd_ = true;
            break;
        }

        // 执行
        currentStmt->execute(vars_, *this);

        if (programEnd_) {
            break;
        }

        // 记录当前PC以判断是否修改
        int currentPC = programCounter_;
        // 获取下一行号
        int nextLine = recorder_.nextLine(currentPC);

        if (nextLine == -1) {
            programEnd_ = true;
        } else if (programCounter_ == currentPC) {
            // PC未被修改，下一行
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
            std::cout << nextLine << " " << stmt->text() << std::endl;
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