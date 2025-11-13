#include <iostream>
#include <cmath>
#include <vector>

enum class ShapeType {
  CIRCLE=1,
  RECTANGLE=2,
  TRIANGLE=3
};

union ShapeData {
  float radius;
  struct {float width, height;} rectangle;
  struct {float a, b, c; } triangle;
};

struct Shape {
  ShapeType type;
  ShapeData data;
};

float area(const Shape& shape) {
  switch (shape.type) {
    case ShapeType::CIRCLE :
      return shape.data.radius*shape.data.radius*3.14;

    case ShapeType::RECTANGLE :
      return shape.data.rectangle.width*shape.data.rectangle.height;

    case ShapeType::TRIANGLE :
      float a = shape.data.triangle.a;
      float b = shape.data.triangle.b;
      float c = shape.data.triangle.c;
      float p = (a+b+c)/2;
      return std::sqrt(p*(p-a)*(p-b)*(p-c));
  }
}

void printShape(const Shape& shape) {
  switch (shape.type) {
    case ShapeType::CIRCLE :
      std::cout << "Type: circle {r=" << shape.data.radius << "}\n";
      break;
    case ShapeType::RECTANGLE :
      std::cout << "Type: rectangle {width=" << shape.data.rectangle.width << ", height=" << shape.data.rectangle.height<< "}\n";
      break;
    case ShapeType::TRIANGLE :
      float a = shape.data.triangle.a;
      float b = shape.data.triangle.b;
      float c = shape.data.triangle.c;
      std::cout << "Type: triangle {a=" << a << ", b=" << b << ", c=" << c << "}\n";
      break;
  }
}

int main() {

  Shape Something_cool_maybe_yes_idk_lol;
  Something_cool_maybe_yes_idk_lol.type = ShapeType::TRIANGLE;
  Something_cool_maybe_yes_idk_lol.data.triangle.a = 3;
  Something_cool_maybe_yes_idk_lol.data.triangle.b = 4;
  Something_cool_maybe_yes_idk_lol.data.triangle.c = 5;

  printShape(Something_cool_maybe_yes_idk_lol);
  std::cout << area(Something_cool_maybe_yes_idk_lol);

  Shape s1;
  s1.type = ShapeType::CIRCLE;
  s1.data.radius = 10.0f;

  Shape s2;
  s2.type = ShapeType::RECTANGLE;
  s2.data.rectangle.width = 4.0f;
  s2.data.rectangle.height = 6.0f;

  Shape s3;
  s3.type = ShapeType::TRIANGLE;
  s3.data.triangle.a = 3.0f;
  s3.data.triangle.b = 4.0f;
  s3.data.triangle.c = 5.0f;

  std::vector<Shape> shapes = {s1, s2, s3};

  for (const auto& s : shapes) {
    printShape(s);
    std::cout << "area = " << area(s) << "\n";
  }

  float total = 0.0f;
  for (const auto& s : shapes) {
    total += area(s);
  }
  std::cout << "Total area = " << total << "\n";



  return 67;
}