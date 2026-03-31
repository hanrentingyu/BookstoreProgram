#include "book.hpp"

#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

// --- 内部工具函数 (匿名命名空间) ---

namespace {
constexpr int kIsbnLength = 10;
constexpr int kIsbnModulus = 11;

// 校验 ISBN-10 格式
bool IsValidIsbn10(const std::string& isbn) {
  if (isbn.length() != kIsbnLength) return false;

  int checksum = 0;
  for (int i = 0; i < kIsbnLength - 1; ++i) {
    if (!std::isdigit(static_cast<unsigned char>(isbn[i]))) return false;
    checksum += (isbn[i] - '0') * (kIsbnLength - i);
  }

  const char last_char = isbn.back();
  if (last_char == 'X' || last_char == 'x') {
    checksum += 10;
  } else if (std::isdigit(static_cast<unsigned char>(last_char))) {
    checksum += (last_char - '0');
  } else {
    return false;
  }

  return (checksum % kIsbnModulus) == 0;
}
}  // namespace

// --- 类成员函数实现 ---

Book::Book() : m_isbn(), m_quantity(0), m_price(0.0) {}

Book::Book(std::string isbn, unsigned int quantity, long double price)
    : m_isbn(std::move(isbn)), m_quantity(quantity), m_price(price) {
  Validate();
}

const std::string& Book::GetIsbn() const noexcept {
  return m_isbn;
}
unsigned int Book::GetQuantity() const noexcept {
  return m_quantity;
}
long double Book::GetPrice() const noexcept {
  return m_price;
}

void Book::SetQuantity(unsigned int qty) {
  m_quantity = qty;
  if (m_quantity == 0 && m_price > 0.0) {
    m_price = 0.0;
  }
}

void Book::SetPrice(long double price) {
  if (m_quantity == 0 && price > 0.0) {
    throw std::invalid_argument("数量为零时，单价必须为零");
  }
  m_price = price;
}

Book& Book::operator+=(const Book& other) {
  if (m_isbn != other.m_isbn) {
    throw std::runtime_error("无法合并不同 ISBN 的书籍");
  }
  if (other.m_quantity == 0) return *this;

  const long double total_value = (m_price * m_quantity) + (other.m_price * other.m_quantity);
  m_quantity += other.m_quantity;
  m_price = (m_quantity == 0) ? 0.0 : (total_value / m_quantity);
  return *this;
}

Book Book::operator+(const Book& other) const {
  Book result = *this;
  result += other;
  return result;
}

bool Book::IsEmpty() const noexcept {
  return m_isbn.empty() && m_quantity == 0 && m_price == 0.0;
}

void Book::Validate() {
  if (m_isbn.empty()) {
    throw std::invalid_argument("ISBN 不能为空");
  }
  if (!IsValidIsbn10(m_isbn)) {
    throw std::runtime_error("无效的 ISBN-10 格式: " + m_isbn);
  }
  if (m_quantity == 0 && m_price > 0.0) {
    throw std::invalid_argument("数量为零时，单价必须为零");
  }
}

// --- 流操作符实现 ---

std::istream& operator>>(std::istream& is, Book& book) {
  std::string temp_isbn;
  unsigned int temp_qty = 0;
  long double temp_price = 0.0;

  if (!(is >> temp_isbn >> temp_qty >> temp_price)) {
    return is;
  }

  try {
    Book temp_book(temp_isbn, temp_qty, temp_price);
    book = temp_book;
  } catch (const std::exception& e) {
    is.setstate(std::ios::failbit);
  }

  return is;
}

std::ostream& operator<<(std::ostream& os, const Book& book) {
  std::ios state(nullptr);
  state.copyfmt(os);

  os << std::fixed << std::setprecision(2) << book.m_isbn << " " << book.m_quantity << " "
     << book.m_price;

  os.copyfmt(state);
  return os;
}