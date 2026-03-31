#ifndef BOOK_HPP
#define BOOK_HPP

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

class Book {
private:
  std::string m_isbn;
  unsigned int m_quantity;
  long double m_price;

public:
  Book();
  explicit Book(std::string isbn, unsigned int quantity = 0, long double price = 0.0);

  // --- 访问器 ---
  const std::string& GetIsbn() const noexcept;
  unsigned int GetQuantity() const noexcept;
  long double GetPrice() const noexcept;

  // --- 修改器 ---
  void SetQuantity(unsigned int qty);
  void SetPrice(long double price);

  // --- 运算符重载 ---
  Book& operator+=(const Book& other);
  Book operator+(const Book& other) const;
  bool IsEmpty() const noexcept;

  // --- 流操作符 ---
  friend std::istream& operator>>(std::istream& is, Book& book);
  friend std::ostream& operator<<(std::ostream& os, const Book& book);

private:
  // --- 内部校验 ---
  void Validate();
};

#endif