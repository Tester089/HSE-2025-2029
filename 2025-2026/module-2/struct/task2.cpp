#include <iostream>
#include <vector>


enum class ParamType {
  INT=1,
  DOUBLE=2,
  BOOL=3,
  STRING=4
};

union ParamValue {
  int i;
  double d;
  bool b;
  char s[64];
};

struct ConfigEntry {
  std::string name;
  ParamType type;
  ParamValue value;
};

void setInt(ConfigEntry& entry, int i) {
  entry.type = ParamType::INT;
  entry.value.i=i;
}
void setDouble(ConfigEntry& entry, double d) {
  entry.type = ParamType::DOUBLE;
  entry.value.d=d;
}
void setBool(ConfigEntry& entry, bool b) {
  entry.type = ParamType::BOOL;
  entry.value.b=b;
}
void setString(ConfigEntry& entry, const char* s) {
  entry.type = ParamType::STRING;
  for (int i = 0; i < 64; i++) {
    entry.value.s[i] = s[i];
  }
}

void printEntry(const ConfigEntry& entry) {
  std::cout << entry.name << ". Value: ";
  switch (entry.type) {
    case ParamType::INT :
        std::cout << entry.value.i << "\n";
        break;
      case ParamType::DOUBLE :
        std::cout << entry.value.d << "\n";
        break;
      case ParamType::BOOL :
        std::cout << entry.value.b << "\n";
        break;
      case ParamType::STRING :
        std::cout << entry.value.s << "\n";
        break;
    }
}


ConfigEntry* findByName(std::vector<ConfigEntry>& config, const std::string& name) {
  for (auto& line : config) {
    if (line.name == name) return &line;
  }
}


int main() {
  ConfigEntry e1{"max_connections", ParamType::INT, {}};
  setInt(e1, 100);

  ConfigEntry e2{"timeout", ParamType::DOUBLE, {}};
  setDouble(e2, 2.5);

  ConfigEntry e3{"use_cache", ParamType::BOOL, {}};
  setBool(e3, true);

  ConfigEntry e4{"log_file", ParamType::STRING, {}};
  setString(e4, "server.log");

  std::vector<ConfigEntry> config = {e1, e2, e3, e4};

  for (const auto& e : config) {
    printEntry(e);
  }

  // Меняем значение параметра timeout
  ConfigEntry* timeoutEntry = findByName(config, "timeout");
  if (timeoutEntry) {
    setDouble(*timeoutEntry, 3.0);
  }

  std::cout << "\nПосле изменения timeout:\n";
  for (const auto& e : config) {
    printEntry(e);
  }

}