// TODO: Imply interfaces declared in the Recorder.hpp.
#include "Recorder.hpp"

Recorder::~Recorder() {
    clear(); 
}

// 插入语句
void Recorder::add(int line, Statement* stmt) {
  // 若已存在先删旧的
  if (hasLine(line)) {
    delete statements_[line];
  }
  statements_[line] = stmt;
}

// 删除
void Recorder::remove(int line) {
  auto it = statements_.find(line);
  if (it != statements_.end()) {
    delete it->second;
    statements_.erase(it);
  }
}

// 获取指定行（不存在则nullptr）
const Statement* Recorder::get(int line) const noexcept {
  auto it = statements_.find(line);
  return (it != statements_.end()) ? it->second : nullptr;
}

// 检查指定行号是否存在语句
bool Recorder::hasLine(int line) const noexcept {
  return statements_.count(line) > 0;
}

// 清空所有语句并释放内存
void Recorder::clear() noexcept {
  for (auto& [line, stmt] : statements_) {
    delete stmt;
  }
  statements_.clear();
}

// 返回大于指定行号的最小行号（不存在则-1）
int Recorder::nextLine(int line) const noexcept {
  auto it = statements_.upper_bound(line);
  return (it != statements_.end()) ? it->first : -1;
}
