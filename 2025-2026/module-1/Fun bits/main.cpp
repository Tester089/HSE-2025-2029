#include <iostream>
#include <bitset>

int main() {
  for (short num = 0; true; num += 256) {
    std::bitset<sizeof(short) * CHAR_BIT> bits(num);
    std::cout << num << " " << bits.to_string() << '\n';
  }

  return 42;
}