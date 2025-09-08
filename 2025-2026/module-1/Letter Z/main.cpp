#include <iostream>

int main() {

  char letter = 'Z';
  int length;
  const int MinLength = 10;
  do {
    std::cout << "Enter length of Z (min 10): ";
    std::cin >> length;
  } while (length < MinLength);


  for (int x = 0; x < length; x++) {
    for (int y = 0; y < length; y++) {
      if (x <= 1 || x >= length - 2) {
        std::cout << letter;
      } else if (x + y >= length - 2 && x + y <= length) {
        std::cout << letter;
      } else {
        std::cout << " ";
      }

      if (y == length-1) {
        std::cout << std::endl;
      }
    }
  }
  return 42;
}
