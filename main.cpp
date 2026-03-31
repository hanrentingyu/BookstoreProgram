#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "book.hpp"

// --- 内部辅助函数 ---

namespace {
// 从文件加载书籍数据到 Map
bool LoadBooksFromFile(const std::string& filename, std::map<std::string, Book>& inventory) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cout << "[Info] 文件未找到，跳过加载: " << filename << "\n";
    return false;
  }

  std::string line;
  int count = 0;
  while (std::getline(file, line)) {
    if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) continue;

    try {
      std::istringstream iss(line);
      Book book;
      if (iss >> book) {
        inventory[book.GetIsbn()] = book;
        ++count;
      }
    } catch (const std::exception& e) {
      std::cerr << "[Warn] 跳过无效行 (" << filename << "): " << e.what() << "\n";
    }
  }

  std::cout << "[Info] 已从 " << filename << " 加载 " << count << " 条记录。\n";
  return true;
}

// 保存 Map 数据到文件
void SaveBooksToFile(const std::string& filename, const std::map<std::string, Book>& inventory) {
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("无法写入文件: " + filename);
  }

  file << std::fixed << std::setprecision(2);
  for (const auto& book : inventory) {
    file << book.second << "\n";
  }

  std::cout << "[Info] 已保存 " << inventory.size() << " 条记录到 " << filename << "\n";
}
}  // namespace

// --- 主程序逻辑 ---

int main() {
  std::map<std::string, Book> inventory;

  std::cout << "=== 开始合并库存数据 ===\n";

  // 1. 加载历史库存
  LoadBooksFromFile("output.txt", inventory);

  // 2. 处理新进货
  std::ifstream input_file("input.txt");
  if (input_file.is_open()) {
    std::string line;
    int processed_count = 0;

    while (std::getline(input_file, line)) {
      if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos) continue;

      try {
        std::istringstream iss(line);
        Book new_book;
        if (!(iss >> new_book)) continue;

        ++processed_count;
        const std::string& isbn = new_book.GetIsbn();

        auto it = inventory.find(isbn);
        if (it != inventory.end()) {
          // 存在则合并
          it->second += new_book;
          std::cout << "[Update] " << isbn << " (新增: " << new_book.GetQuantity()
                    << ", 总计: " << it->second.GetQuantity() << ")\n";
        } else {
          // 不存在则插入
          inventory.emplace(isbn, new_book);
          std::cout << "[New]    " << isbn << " (数量: " << new_book.GetQuantity() << ")\n";
        }
      } catch (const std::exception& e) {
        std::cerr << "[Error] 处理输入行失败: " << e.what() << "\n";
      }
    }
    std::cout << "[Info] 输入文件处理完毕，共处理 " << processed_count << " 条有效记录。\n";
  } else {
    std::cout << "[Warn] 未找到 input.txt，仅保留历史库存。\n";
  }

  // 3. 持久化保存
  try {
    SaveBooksToFile("output.txt", inventory);
    std::cout << "=== 操作成功完成 ===\n";
  } catch (const std::exception& e) {
    std::cerr << "[Fatal] 保存失败: " << e.what() << "\n";
    return 1;
  }

  return 0;
}