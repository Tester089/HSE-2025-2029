#include <iostream>
#include <vector>

enum struct Statuses {
  NONE=0,
  CREATED=1,
  PAID=2,
  PROCESSING=3,
  SHIPPED=4,
  DELIVERED=5,
  CANCELLED=6,
};


void changeStatus(std::vector<char>& order, Statuses status) {
  order[(int)status] ^= 1;
}


int main() {

  std::vector<std::vector<char>> orders = {
    {1,0,1,0,1,0,1},
    {0,1,0,1,0,1,0},
    {1,0,1,0,1,0,1},
    {0,1,0,1,0,1,0},
    {1,0,1,0,1,0,1},
    {0,1,0,1,0,1,0},
    {1,0,1,0,1,0,1},
    {0,1,0,1,0,1,0},
    {1,0,1,0,1,0,1},
    {0,1,0,1,0,1,0},
    {1,0,1,0,1,0,1},
    {0,1,0,1,0,1,0},
    };

  int index = -1;
  changeStatus(orders[0], Statuses::CREATED);
  changeStatus(orders[1], Statuses::PAID);
  for (auto& order : orders) {
    index++;
    if (order[(int)Statuses::CREATED] && order[(int)Statuses::PAID]) {
      std::cout << index << "\n";

    }
  }


}