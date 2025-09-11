#include <iostream>

// Automatic (stack)

int auto_var() {
  std::cout << "--- Automatic Demo ---" << std::endl;

  int var_a = 10;
  std::cout << "Before block: var_a = " << var_a << std::endl;

  {
    int var_b = 20;
    var_a = 99;
    std::cout << "Inside block: var_a = " << var_a << std::endl;
    std::cout << "Inside block: var_b = " << var_b << std::endl;
  }

  std::cout << "After block: var_a = " << var_a << std::endl;

  return 0;
}


// Statick (global)

void demo() {
  int automatic_var = 0;
  static int static_var = 0;

  automatic_var++;
  static_var++;

  std::cout << "Automatic: " << automatic_var;
  std::cout << " | Static: " << static_var << std::endl;
}

int var_statick() {
  std::cout << "--- Static Demo ---" << std::endl;
  demo();
  demo();
  demo();

  return 0;
}


// Dynamic  (heap)

int* createNumberOnHeap(int value) {
  int* ptr = new int(value);
  return ptr;
}

int var_dynamic() {
  std::cout << "--- Dynamic Demo ---\n";

  int* my_heap_number = nullptr;

  my_heap_number = createNumberOnHeap(42);

  std::cout << "Function createNumberOnHeap() is finished.\n";
  std::cout << "But the number it created still exists: " << *my_heap_number << "\n";

  delete my_heap_number;
  my_heap_number = nullptr;

  std::cout << "Manualy freed the memory.\n";

  return 0;
}


int main() {
  auto_var();
  std::cout << "\n";

  var_statick();
  std::cout << "\n";

  var_dynamic();
  std::cout << "\n";

  return 42;

}