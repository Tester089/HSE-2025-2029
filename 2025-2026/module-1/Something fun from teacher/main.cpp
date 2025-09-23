#include <iostream>

int main() {
  bool s = true;
  std::cout << sizeof(0) << " " << sizeof(s) << " " << sizeof(0L) << " " << sizeof(0LL) << "\n";
  std::cout << (-1 + 0u);
}