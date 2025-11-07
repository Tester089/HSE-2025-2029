#include <iostream>
#include <vector>


void write(std::vector<int>* matrix, size_t n) {
  for (size_t i = 0; i < n; i++) {
    int temp;
    std::cin >> temp;
    (*matrix)[i] = temp;
  }
}

void print(std::vector<int>* matrix) {
  for (auto element : *matrix) {
    std::cout << element << " ";
  }
  std::cout << "\n";
}

bool isPositive(int x) {
  if (x > 0) {
    return 1;
  }
  return 0;
}

bool isEven(int x) {
  if ((x >> 1) << 1 == x) {
    return 1;
  }
  return 0;
}


bool isGood(int x) {
  return !isEven(x) && isPositive(x);
}


std::vector<int> filterArray(
    const std::vector<int>& arr,
    bool (*filter)(int),
    int& resultSize
) {

  size_t arrSize = arr.size();
  std::vector<int> filteredMatrix;
  for (size_t i = 0; i < arrSize; i++) {
    {
      if (!filter(arr[i])) {
        filteredMatrix.push_back(arr[i]);
        resultSize++;
      }
    }

  }
  return filteredMatrix;
}

int main() {
  size_t n;
  std::cin >> n;
  std::vector<int> original(n);
  write(&original, n);
  int someint = 0;
  std::vector<int> edited = filterArray(original, isGood, someint);

  for (size_t i = 0; i < n; i++) {
    for (auto black : edited) {
      if (black == original[i]) {
        original.erase(original.begin()+i);
        i--;
        n--;
      }
    }
  }s

  print(&original);
  print(&edited);

}