#include <algorithm>
#include <iostream>
#include <vector>


std::vector<std::vector<int>> generate_matrix(int n, int m) {
  std::vector<std::vector<int>> arr(n, std::vector<int>(m));
  for (auto& line : arr) {
    for (auto& element : line) {
      std::cin >> element;
    }
  }
  return arr;
}


void print_matrix(const std::vector<std::vector<int>>& mat) {
  for (auto& line : mat) {
    for (auto& element : line) {
      std::cout << element << " ";
    }
    std::cout << "\n";
  }
}


std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& mat) {
  std::vector<std::vector<int>> arr_t(mat[0].size(), std::vector<int>(mat.size()));
  for (int x = 0; x < mat.size() ; x++) {
    for (int y = 0; y < mat[0].size() ; y++) {
      arr_t[y][x] = mat[x][y];
    }
  }
  return arr_t;
}

void cube_positive(std::vector<std::vector<int>>& mat) {
  for (auto& line : mat) {
    for (auto& element : line) {
      if (element > 0) {
        element *= element;
      }
    }
  }
}


std::pair<std::vector<int>, long long> column_maxima_and_negative_product(const std::vector<std::vector<int>>& mat) {
  std::vector<int> part_one;
  long long part_two = 1;

  for (auto& line : mat) {
    int maximal = line[0];
    for (auto& element : line) {
      if (element > maximal) {
        maximal = element;
      }
      if (element < 0) {
        part_two *= (long long)element * (long long)element;
      }
    }
    part_one.push_back(maximal);
  }
  std::pair<std::vector<int>, long long> output(part_one, part_two);
  return output;
}


template<class ForwardIt, class Pred, class UnaryOp>
void apply_if_range(ForwardIt first, ForwardIt last, Pred p, UnaryOp op) {
  for (auto it = first; it != last; ++it) {
    if (p(*it)) {
      *it = op(*it);
    }
  }
}


template<class InputIt, class OutputItTrue, class OutputItFalse, class Pred>
std::pair<OutputItTrue, OutputItFalse> partition_copy_range(
    InputIt first, InputIt last,
    OutputItTrue out_true, OutputItFalse out_false, Pred p) {

  for (auto it = first; it != last; ++it) {
    if (p(*it)) {
      *out_true = *it;  // Присваивание через итератор
      ++out_true;       // Инкремент итератора
    } else {
      *out_false = *it; // Присваивание через итератор
      ++out_false;      // Инкремент итератора
    }
  }

  return std::pair<OutputItTrue, OutputItFalse>(out_true, out_false);
}


int main() {

  int n, m;
  std::cin >> n >> m;

  auto arr = generate_matrix(n, m);

  auto arr_t = transpose(arr);
  std::cout << "\nTask 1 \n";
  print_matrix(arr_t);

  print_matrix(arr);

  std::cout << "\nTask 2 \n";

  cube_positive(arr);
  print_matrix(arr);

  std::cout << "\nTask 3 \n";

  auto ans_3 = column_maxima_and_negative_product(arr);
  for (auto& element : ans_3.first) {
    std::cout << element << " ";
  } std::cout << "\n";

  std::cout << ans_3.second;

  std::cout << "\nTask 4 \n";

  apply_if_range(arr[0].begin(), arr[arr.size()-1].end(), [](int a){return a%3==0;}, [](int a){return a+5;});

  print_matrix(arr);

  std::cout << "\nTask 5 \n";
  std::vector<int> arr_temp_1;
  std::vector<int> arr_temp_2;

  auto ans_5 = partition_copy_range(arr[0].begin(), arr[arr.size()-1].end(), arr_temp_1, arr_temp_2, [](int x){return (x >= 0);});


  for (auto& element : ans_5.first) {
    std::cout << element << " ";
  }
  std::cout << "\n";

  for (auto& element : ans_5.second) {
    std::cout << element << " ";
  }
  std::cout << "\n";


}