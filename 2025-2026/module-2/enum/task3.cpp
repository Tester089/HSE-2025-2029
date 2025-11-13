#include <iostream>
#include <string>


enum Emotion {
  smile=1,
  sad=2,
  funky=3
};

enum class Fealings {
  smile=11,
  NotSad=22,
  funky=33
};



// For point 2
// we cannot add new enum with the same names
// enum Emotion_v2 {
//   smile=4,
//   sad=5,
//   funky=6
// };


// For point 3
// but HERE we CAN create enum with the same names. Yay!
enum class Fealings_v2 {
  smile=44,
  NotSad=55,
  funky=66
};



int main() {

  // Issues of regular enum

  Emotion a1 = smile;
  Emotion a2 = sad;

  std::cout << a1 << " " << a2 << "\n";

  Fealings b1 = Fealings::smile;
  Fealings b2 = Fealings::NotSad;

  std::cout << static_cast<int>(b1) << " " << static_cast<int>(b2) << "\n";

  Fealings_v2 b1_v2 = Fealings_v2::smile;
  Fealings_v2 b2_v2 = Fealings_v2::NotSad;

  std::cout << static_cast<int>(b1_v2) << " " << static_cast<int>(b2_v2) << "\n";

  return 42;
}