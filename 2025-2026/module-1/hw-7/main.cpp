// #include <iostream>
// #include <set>
//
// bool is_good(std::set<char>& alphabet, std::set<char>& word) {
//   for (auto& letter_word : word) {
//     bool is_founded = 0;
//     for (auto& letter_alph : alphabet) {
//       if (letter_alph == letter_word) is_founded = 1;
//     }
//     if (!is_founded) return 0;
//   }
//   return 1;
// }
//
// std::set<char> string_to_line(std::string line) {
//   std::set<char> base;
//   for (auto& letter : line) {
//     base.insert(letter);
//   }
//   return base;
// }
//
// int main() {
//   std::string line;
//   std::cin >> line;
//   auto alphabet = string_to_line(line);
//
//   size_t iterations;
//   std::cin >> iterations;
//   for (size_t i = 0; i < iterations; i++) {
//     std::string line;
//     std::cin >> line;
//     auto word = string_to_line(line);
//     if (is_good(alphabet, word)) std::cout << line << "\n";
//   }
// }


// #include <iostream>
// #include <map>
// #include <set>
// #include <string>
//
// int main() {
//   size_t n;
//   std::cin >> n;
//   std::map<size_t, std::set<std::string>> base;
//   for (size_t i = 0; i < n; i++) {
//     std::string word;
//     size_t page;
//     std::cin >> word >> page;
//     base[page].insert(word);
//   }
//   for (const auto& line : base) {
//     std::cout << line.first << " ";
//     for (const auto& word : line.second) {
//       std::cout << word << " ";
//     }
//     std::cout << "\n";
//   }
// }



// #include <iostream>
// #include <map>
// #include <vector>
// #include <string>
//
//
//
// int main() {
//   size_t iterations;
//   std::cin >> iterations;
//   std::map<std::string, std::vector<size_t>> base;
//   for (size_t i = 0; i < iterations; i++) { // count of lines
//     size_t persons;
//     std::cin >> persons;
//     for (size_t ii = 0; ii < persons; ii++) {
//       std::string name;
//       size_t coins;
//       std::cin >> name >> coins;
//       if (base.find(name) != base.cend()) {
//         base[name][i] = coins;
//       } else {
//         base[name] = std::vector<size_t>(iterations);
//         base[name][i] = coins;
//       }
//
//     }
//
//   }
//
//
//   for (auto& line : base) {
//     std::cout << line.first << "\t";
//     for (auto& coin : line.second) {
//       std::cout << coin << "\t";
//     }
//     std::cout << "\n";
//   }
//
//
// }


// #include <iostream>
// #include <map>
// #include <set>
// #include <string>
// #include <sstream>
// #include <vector>
//
//
// int main() {
//   std::map<std::string, std::set<std::string>> base;
//   std::string line;
//   while (std::getline(std::cin, line) && !line.empty()) {
//     for (auto& letter : line) {
//       if (letter == '/') letter = ' ';
//     }
//     std::stringstream s(line);
//     std::vector<std::string> items;
//     std::string item;
//     while (s >> item) {
//       items.push_back(item);
//     }
//
//
//     for (size_t i = 1; i < items.size(); i++) {
//       base[items[i-1]].insert(items[i]);
//     }
//
//   }
//
//   std::cout << "/\n";
//   for (auto& items : base) {
//     std::cout << "/" << items.first << "/\n";
//     for (auto& item : items.second) {
//       if (base.find(item) != base.end()) {
//         std::cout << "/" << items.first << "/" << item << "/\n";
//       }
//     }
//   }
//
//
// }


// #include <iostream>
// #include <string>
// #include <set>
//
// int main() {
//   std::string line;
//   std::set<std::string> base;
//   while (std::getline(std::cin, line) && !line.empty()) {
//
//     std::string temp;
//     for (auto& letter : line) {
//       temp += letter;
//       if (letter ==  '/') {
//         base.insert(temp);
//       }
//     }
//
//   }
//
//   for (auto& element : base) {
//     std::cout << element << "\n";
//   }
// }


#include <iostream>

int main() {
  size_t numbers, window;
  std::cin >> numbers >> window;
  int number;
  std::cin >> number;
  for (size_t i = 1; i < window; i++) {
    int temp;
    std::cin >> temp;
    if (temp < number) number = temp;
  }
  std::cout << number << "\n";
  for (size_t i = window-1; i < numbers; i++) {
    int temp;
    std::cin >> temp;
    if (temp < number) number = temp;
    std::cout << number << "\n";
  }

}