#include <iostream>
#include <cmath>

class Vector2D {
public:
  double x;
  double y;

  struct Polar {
    double r;
    double angle;
  };

  Vector2D(double x, double y) {
    this->x = x;
    this->y = y;
  }

  Vector2D &scale(double k) {
    this->x *= k;
    this->y *= k;
    return *this;
  }

  Polar toPolar() const {
    return Polar{std::pow(std::pow(this->x, 2) + std::pow(this->y, 2), 0.5), std::atan2(this->y, this->x)};
  }

  Vector2D &add(double dx, double dy) {
    this->x += dx;
    this->y += dy;
    return *this;
  }
};

void printVector(const Vector2D &omg) {
  std::cout << omg.x << ' ' << omg.y << '\n';
}

void printPolar(const Vector2D::Polar &omgx2) {
  std::cout << omgx2.r << ' ' << omgx2.angle << '\n';
}

int main() {
  Vector2D v{3.0, 4.0};

  v.add(1.0, -2.0);
  v.scale(2.0);
  v.add(-3.0, 1.0);

  auto p = v.toPolar();

  std::cout << "В декартовой форме: ";
  printVector(v);
  std::cout << "В полярной форме: ";
  printPolar(p);
}
