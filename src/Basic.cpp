#include <cctype>
#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Token.hpp"
#include "utils/Error.hpp"

// 辅助函数：去除字符串前后的空格
static std::string trim(const std::string& s) {
  auto start = s.begin();
  while (start != s.end() && std::isspace(static_cast<unsigned char>(*start))) {
    start++;
  }

  auto end = s.end();
  do {
    end--;
  } while (std::distance(start, end) > 0 &&
           std::isspace(static_cast<unsigned char>(*end)));

  return std::string(start, end + 1);
}

// 打印帮助信息
static void printHelp() {
  std::cout << "Supported commands:\n";
  std::cout << "  General statements (with line number):\n";
  std::cout << "    REM <comment> - Comment line (ignored)\n";
  std::cout
      << "    LET <var> = <expr> - Assign expression result to variable\n";
  std::cout << "    PRINT <expr> - Evaluate expression and print result\n";
  std::cout
      << "    INPUT <var> - Prompt with '?', read integer into variable\n";
  std::cout << "    END - Terminate program execution\n";
  std::cout << "    GOTO <line> - Jump to specified line number\n";
  std::cout << "    IF <expr1> <op> <expr2> THEN <line> - Conditional jump "
               "(op: =, <, >)\n";
  std::cout
      << "  Immediate execution (without line number): LET, PRINT, INPUT\n";
  std::cout << "  Interpreter commands:\n";
  std::cout << "    RUN - Execute program from lowest line number\n";
  std::cout << "    LIST - Display all program lines in order\n";
  std::cout << "    CLEAR - Remove all program lines\n";
  std::cout << "    QUIT - Exit the interpreter\n";
  std::cout << "    HELP - Show this help message\n";
}

int main() {
  Lexer lexer;
  Parser parser;
  Program program;

  std::string line;
  while (std::getline(std::cin, line)) {
    std::string trimmedLine = trim(line);
    if (trimmedLine.empty()) {
      continue;
    }

    // 优先处理解释器指令
    if (trimmedLine == "QUIT") {
      break;
    } else if (trimmedLine == "RUN") {
      program.run();
      continue;
    } else if (trimmedLine == "LIST") {
      program.list();
      continue;
    } else if (trimmedLine == "CLEAR") {
      program.clear();
      continue;
    } else if (trimmedLine == "HELP") {
      printHelp();
      continue;
    }
    // 处理其他语句（带行号的语句和立即执行的LET/PRINT/INPUT）
    try {
      TokenStream tokens = lexer.tokenize(line);
      if (tokens.empty()) {
        continue;
      }
      ParsedLine parsedLine = parser.parseLine(tokens, line);
      auto lineNumber = parsedLine.getLine();
      Statement* stmt = parsedLine.fetchStatement();

      if (lineNumber.has_value()) {
        // 带行号的语句：若语句为空，删除；否则添加更新
        if (stmt == nullptr) {
          program.removeStmt(lineNumber.value());
        } else {
          program.addStmt(lineNumber.value(), stmt);
        }
      } else {
        // 立即执行语句：直接执行并释放内存
        if (stmt != nullptr) {
        try {
            program.execute(stmt);
        } catch (const BasicError& e) {
            delete stmt;
            throw e;
        }
        delete stmt;
        stmt = nullptr;
    }
      }
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
    }
  }
  return 0;
}