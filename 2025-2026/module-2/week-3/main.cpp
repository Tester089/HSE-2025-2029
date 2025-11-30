#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
using namespace std; // BAD
// 1) Create a triangle structure (a, b, c) with a constructor, output method, and area method.
// 2) Create a vector of 20 objects (any values).
// For each, output the area and find the triangle with the largest area.
// 3) Create a vector (push_back) of 1000 triangle values ​​by pointers and references.
//
struct Triangle {
  int a;
  int b;
  int c;
  vector<int> dummy;
public:
  Triangle(int a = 0, int b = 0, int c = 0) {
    this->a = a;
    this->b = b;
    this->c = c;
    dummy = vector<int>(100000);
  }
  void print() {
    std::cout << a << ' ' << b << ' ' << c << '\n';
  }
  double square() {
    double p = (a + b + c) / 2.0;
    return sqrt(p * (p - a) * (p - b) * (p - c));
  }
};
// Compare passing to methods.
// 1) passing by value
// 2) by reference
// 3) by pointer
// Do this 10,000 times and print the time

void pass_by_value(Triangle t) { }
void pass_by_ref(Triangle& t) { }
void pass_by_p(Triangle* t) { }



int main() {
  std::cout << "=== Task 1 and 2 ===\n";
  std::vector<Triangle> triangles;

  for  (int i = 0; i < 20; i++) {
    triangles.emplace_back(i+3, 4+i, 5+i/2);
  }


  double maxArea = 0;
  int maxIndex = 0;

  std::cout << "Areas:\n";
  for (int i = 0; i < triangles.size(); i++) {
    double area = triangles[i].square();
    std::cout << "triange " << i << " has area: " << area << "\n";

    if (area>maxArea) {
      maxArea = area;
      maxIndex = i;
    }
  }

  std::cout << "Max area has triange " << maxIndex << " and its area: " << maxArea << "\n";

  std::cout << "\n=== Task 3 ===";

  std::vector<Triangle> triangle_objects;
  triangle_objects.reserve(100);

  std::vector<Triangle*> triangle_pointers;
  triangle_pointers.reserve(1000);

  for (int i = 0; i < 1000; i++) {
    triangle_objects.emplace_back(5 + (i % 10), 6 + (i % 10), 7 + (i % 10));
    triangle_pointers.push_back(new Triangle(5 + (i % 10), 6 + (i % 10), 7 + (i % 10)));
  }


  Triangle testTriangle(5,6,7);
  int iterations = 10000;


  auto start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iterations; i++) {
    pass_by_value(testTriangle);
  }
  auto end = std::chrono::high_resolution_clock::now();
  auto duration_value = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
  
}