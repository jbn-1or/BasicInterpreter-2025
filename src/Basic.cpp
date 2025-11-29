#include <iostream>
#include <memory>
#include <string>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Token.hpp"
#include "utils/Error.hpp"

int main() {
  Lexer lexer;
  Parser parser;
  Program program;

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    try {
      // TODO: The main function.
      // 1. 词法分析：将输入行转换为令牌流
      TokenStream tokens = lexer.tokenize(line);
      if (tokens.empty()) {
        continue;
      }

      // 2. 语法分析：解析令牌流为 ParsedLine（包含行号和语句）
      ParsedLine parsedLine = parser.parseLine(tokens, line);

      // 3. 处理解析结果
      auto lineNumber = parsedLine.getLine();
      Statement* stmt = parsedLine.fetchStatement();

      if (lineNumber.has_value()) {
        // 带行号的语句：若语句为空，代表删除该行；否则添加/更新该行
        if (stmt == nullptr) {
          program.removeStmt(lineNumber.value());
        } else {
          program.addStmt(lineNumber.value(), stmt);
        }
      } else {
        // 立即执行语句：直接执行并释放内存
        if (stmt != nullptr) {
          program.execute(stmt);
          delete stmt;
        }
      }
    } catch (const BasicError& e) {
      std::cout << e.message() << "\n";
    }
  }
  return 0;
}