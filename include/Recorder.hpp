#pragma once

#include <map>
#include <memory>
#include <vector>

#include "Statement.hpp"

class Recorder {
 public:
  ~Recorder() { clear(); }

  // 插入语句
  void add(int line, Statement* stmt) {
    // 若已存在先删旧的
    if (hasLine(line)) {
      delete statements_[line];
    }
    statements_[line] = stmt;
  }

  // 删除
  void remove(int line) {
    auto it = statements_.find(line);
    if (it != statements_.end()) {
      delete it->second;
      statements_.erase(it);
    }
  }

  // 获取指定行（不存在则nullptr）
  const Statement* get(int line) const noexcept {
    auto it = statements_.find(line);
    return (it != statements_.end()) ? it->second : nullptr;
  }

  // 检查指定行号是否存在语句
  bool hasLine(int line) const noexcept {
    return statements_.count(line) > 0;
  }

  // 清空所有语句并释放内存
  void clear() noexcept {
    for (auto& [line, stmt] : statements_) {
      delete stmt;
    }
    statements_.clear();
  }

  // 按行号升序输出
  void printLines() const;

  // 返回大于指定行号的最小行号（不存在则-1）
  int nextLine(int line) const noexcept {
    auto it = statements_.upper_bound(line);
    return (it != statements_.end()) ? it->first : -1;
  }

 private:
  // 行号与语句
  std::map<int, Statement*> statements_;
};