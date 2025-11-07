#include <iostream>
#include <vector>
#include <functional>

std::vector<std::vector<int>> generate_matrix(int n, int m) {
  std::vector<std::vector<int>> arr(n, std::vector<int>(m));
  for (auto& line : arr) {
    for (auto& element : line) {
      std::cin >> element;
    }
  }
  return arr;
}


void print_matrix(std::vector<std::vector<int>> arr) {
  for (auto& line : arr) {
    for (auto& element : line) {
      std::cout << element << " ";
    }
    std::cout << "\n";
  }
}

// task1 - 55:30
// task2 - 49:43
// tasl3 - 45:19
// task4 - 23:08 (прибыл Годжу Сатор)
// task5 - 19:18


std::vector<std::vector<int>> add_matrices(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B) {
  int n = A.size();
  int m = A[0].size();

  std::vector<std::vector<int>> arr(n, std::vector<int>(m));

  for (int i=0; i < n; i++) {
    for (int j=0; j < m; j++) {
      arr[i][j] = A[i][j] + B[i][j];
    }
  }
  return arr;
}

std::vector<std::vector<int>> sub_matrices(const std::vector<std::vector<int>>& A, const std::vector<std::vector<int>>& B) {
  int n = A.size();
  int m = A[0].size();

  std::vector<std::vector<int>> arr(n, std::vector<int>(m));

  for (int i=0; i < n; i++) {
    for (int j=0; j < m; j++) {
      arr[i][j] = A[i][j] - B[i][j];
    }
  }
  return arr;
}


size_t count_if(const std::vector<std::vector<int>>& M, std::function<bool(int)> pred) {
  size_t output=  0;
  for (auto& line : M) {
    for (auto& element : line) {
      if (pred(element)) {
        output++;
      }
    }
  }
  return output;
}


template <typename T>
void modify_if(std::vector<std::vector<T>>& M, std::function<bool(T)> pred, std::function<T(T)> transform) {
  for (auto& line : M) {
    for (auto& element : line) {
      if (pred(element)) {
        element = transform(element);
      }
    }
  }
}

template <typename T>
std::function<bool(T)> alpha = [](T x){return x > 0;};
template <typename T>
std::function<T(T)> betta = [](T x){return x * 10;};


template <typename T>
std::vector<std::vector<T>> transpose(const std::vector<std::vector<T>>& M) {
  int n = M.size();
  int m = M[0].size();

  std::vector<std::vector<T>> arr(m, std::vector<int>(n));
  for (int i=0; i < n; i++) {
    for (int j=0; j < m; j++) {
      arr[j][i] = M[i][j];
    }
  }
  return arr;
}



int main() {

  int n, m;
  std::cin >> n >> m;

  std::vector<std::vector<int>> arr_A = generate_matrix(n, m);

  auto FINAL_CRISHA_UZE_EDET_YO_HO_HO = transpose(arr_A);
  print_matrix(FINAL_CRISHA_UZE_EDET_YO_HO_HO);

}
