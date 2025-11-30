#include <iostream>
#include <string>



class User {
public:
  std::string name;
  int id;

  class Address {
  public:
    std::string city;
    std::string street;
    int house;

    std::string toString() {
      return this->city + this->street + this->house;
    }
  };

  Address address;

  User(std::string name, int id, const Address& addr) {
    this->name = name;
    this->id = id;
    this->address = addr;
  }

  User(const User::Address& other) {
    this->address = other;
  }

  void User& operator=(const User& other) {
    this->name = other.name;
    this->id = other.id;
    this->address = other.addr;
  }

  std::string toString(User user_) {
    return "User[name=" + user_.name + ", id=" + std::string(user_.id) + ", address=" + user_.address.toString() + "]\n";
  }

};


int main() {
  User::Address addr{"Moscow", "Tverskaya", 10};
  User u1{"Иван", 1, addr};

  User u2 = u1;   // копирующий конструктор
  u2 = u2;        // self-assignment

  std::cout << u1.toString() << "\n";
  std::cout << u2.toString() << "\n";
}