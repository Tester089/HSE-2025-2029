#include <iostream>
#include <vector>
#include <string>


enum class Permission {
    READ = 1,
    WRITE = 2,
    EXECUTE = 4,
    DELETE = 8
};


Permission operator|(Permission a, Permission b) {
    return static_cast<Permission>(
        static_cast<int>(a) | static_cast<int>(b)
    );
}

Permission operator&(Permission a, Permission b) {
    return static_cast<Permission>(
        static_cast<int>(a) & static_cast<int>(b)
    );
}


bool check_permission(int user_permissions, Permission needed) {
    return (user_permissions & static_cast<int>(needed)) != 0;
}


struct User {
    std::string name;
    int permissions;
};

int main() {

    std::vector<User> users = {
        {"Alice", static_cast<int>(Permission::READ) | static_cast<int>(Permission::WRITE)},
        {"Bob", static_cast<int>(Permission::READ)},
        {"Charlie", static_cast<int>(Permission::READ) | static_cast<int>(Permission::EXECUTE) | static_cast<int>(Permission::DELETE)},
        {"David", 0}  // Нет прав
    };


    for (const auto& user : users) {
        std::cout << user.name << ": " << std::endl;
        std::cout << "  READ: "
                  << check_permission(user.permissions, Permission::READ) << std::endl;

        std::cout << "  WRITE: "
                  << check_permission(user.permissions, Permission::WRITE) << std::endl;

        std::cout << "  EXECUTE: "
                  << check_permission(user.permissions, Permission::EXECUTE) << std::endl;

        std::cout << "  DELETE: "
                  << check_permission(user.permissions, Permission::DELETE) << std::endl;
    }

    return 0;
}