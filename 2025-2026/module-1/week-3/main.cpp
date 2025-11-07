#include <iostream>
#include "function.h"
#include <cmath>
#include "plus.h"
#include "minus.h"
#include "multiply.h"
#include "divide.h"


void task1() {

  std::cout << 'a' << ' ' << int('a') << "\n";
  std::cout << 'b' << ' ' << int('b') << "\n";
  std::cout << 'c' << ' ' << int('c') << "\n";


}
void task2() {

  int a, b, c, d;
  std::cin >> a >> b >> c >> d;
  std::cout << (a + b) * c - d;


}
void task3() {

  print_smth();


}
void task4() {



}
void task5() {

  int a, b;
  std::cin >> a >> b;
  std::cout << (a | b) << '\n';
  std::cout << (a & b) << '\n';
  std::cout << (a ^ b) << '\n';
  std::cout << (a >> 1) << '\n';
  std::cout << (a << 1) << '\n';
  std::cout << (~a) << '\n';

}
void task6() {
  int number, sum_of_digits;
  std::cin >> number;
  sum_of_digits = number % 10;
  number = number / 10;
  sum_of_digits += number % 10;
  number = number / 10;
  sum_of_digits += number % 10;
  number = number / 10;
  sum_of_digits += number % 10;
  number = number / 10;
  std::cout << sum_of_digits << '\n';

}
void task7() {

  int n;
  std::cin >> n;
  std::cout << sqrt(n) << '\n';
  std::cout << sin(n) << '\n';
  std::cout << cos(n) << '\n';
  std::cout << pow(n, 2) << '\n';

}
void task8() {

  

}
void task9() {

}
void task10() {

}
void task11() {

}
void task12() {

}
void task13() {

}
void task14() {

}
void task15() {

}
void task16() {

}


int main() {

  task7();

  return 42;
}