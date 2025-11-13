#include <iostream>
#include <string>

enum class StatusEffect {
  POISONED=1,
  STUNNED=1<<1,
  INVISIBLE=1<<2,
  INVINCIBLE=1<<3,
  BURNING=1<<4,
  FROZEN=1<<5,
  BLESSED=1<<6,
  CURSED=1<<7
};

StatusEffect operator & (StatusEffect a, StatusEffect b) {
  return static_cast<StatusEffect>(static_cast<int>(a)&static_cast<int>(b));
}

StatusEffect operator | (StatusEffect a, StatusEffect b) {
  return static_cast<StatusEffect>(static_cast<int>(a)|static_cast<int>(b));
}


struct Character {
  std::string name;
  std::string user_name;
  StatusEffect effects;
};


bool isCollisionWithEffect(Character this_hero, StatusEffect new_effect) {
  if (static_cast<int>(this_hero.effects & StatusEffect::BURNING) == static_cast<int>(StatusEffect::BURNING)) {
    return new_effect == StatusEffect::FROZEN;
  }

  if (static_cast<int>(this_hero.effects & StatusEffect::FROZEN) == static_cast<int>(StatusEffect::FROZEN)) {
    return new_effect == StatusEffect::BURNING;
  }
  return false;
}


void addEffect(Character &this_hero, StatusEffect new_effect) {
  if (!isCollisionWithEffect(this_hero, new_effect)) {
    this_hero.effects = this_hero.effects | new_effect;
  }
}

void printEffects(Character &character) {
  if (static_cast<int>(character.effects & StatusEffect::POISONED) == static_cast<int>(StatusEffect::POISONED)) {
    std::cout << " POISONED";
  }
  if (static_cast<int>(character.effects & StatusEffect::STUNNED) == static_cast<int>(StatusEffect::STUNNED)) {
    std::cout << " STUNNED";
  }
  if (static_cast<int>(character.effects & StatusEffect::INVISIBLE) == static_cast<int>(StatusEffect::INVISIBLE)) {
    std::cout << " INVISIBLE";
  }
  if (static_cast<int>(character.effects & StatusEffect::INVINCIBLE) == static_cast<int>(StatusEffect::INVINCIBLE)) {
    std::cout << " INVINCIBLE";
  }
  if (static_cast<int>(character.effects & StatusEffect::BURNING) == static_cast<int>(StatusEffect::BURNING)) {
    std::cout << " BURNING";
  }
  if (static_cast<int>(character.effects & StatusEffect::FROZEN) == static_cast<int>(StatusEffect::FROZEN)) {
    std::cout << " FROZEN";
  }
  if (static_cast<int>(character.effects & StatusEffect::BLESSED) == static_cast<int>(StatusEffect::BLESSED)) {
    std::cout << " BLESSED";
  }
  if (static_cast<int>(character.effects & StatusEffect::CURSED) == static_cast<int>(StatusEffect::CURSED)) {
    std::cout << " CURSED";
  }
  std::cout << "\n";
}

int main() {
  std::cout << "\n\n\t\t\t The Game.\n\t\t\t\tReal.\n\n";

  std::cout << "\nUsername, what is your name?: ";
  Character the_hero {};
  std::cin >> the_hero.user_name;

  std::cout << "\n...\nnice to meet you, " << the_hero.user_name << "\n";

  std::cout << "Now I want to hear from You the name of \nYour hero: ";
  std::cin >> the_hero.name;


  addEffect(the_hero, StatusEffect::FROZEN);
  addEffect(the_hero, StatusEffect::BURNING);
  addEffect(the_hero, StatusEffect::BLESSED);

  printEffects(the_hero);



}

