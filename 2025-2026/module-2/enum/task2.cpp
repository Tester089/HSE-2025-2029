#include <vector>
#include <iostream>
#include <string>

enum class Order {
    NONE = 1 << 0,
    CREATED = 1 << 1,
    PAID = 1 << 2,
    PROCESSING = 1 << 3,
    SHIPPED = 1 << 4,
    DELIVERED = 1 << 5,
    CANCELLED = 1 << 6
};

struct Orderu {
    int id;
    std::string name;
    Order ord;
};

Order operator & (Order a, Order b) {
    return static_cast<Order>(static_cast<int>(a)&static_cast<int>(b));
}

std::vector<Orderu> filter(std::vector<Orderu> &orders, Order const status) {
    std::vector<Orderu> ans;
    for (auto ord : orders) {
        if ((ord.ord & status) == status) {
            ans.push_back(ord);
        }
    }
    return ans;
}

void changeByIndex(std::vector<Orderu> &orders, int index, Order replacement) {
    if (index < 0 || index >= orders.size()) {
        std::cout << "The index is wrong";
    }
    else {
        orders[index].ord = replacement;
    }
}

void printStatus(const std::vector<Orderu> &orders) {
    for (auto ord : orders) {
        std::cout << ord.id << "\t";
        switch (ord.ord) {
            case Order::CREATED :
                std::cout << "CREATED\t";
                break;
            case Order::DELIVERED :
                std::cout << "DELIVERED\t";
                break;
            case Order::CANCELLED :
                std::cout << "CANCELLED\t";
                break;
            case Order::PAID :
                std::cout << "PAID\t";
                break;
            case Order::PROCESSING :
                std::cout << "PROCESSING\t";
                break;
            case Order::SHIPPED :
                std::cout << "SHIPPED\t";
                break;
            default:
                std::cout << "NONE\t";
        }
        std::cout << '\n';
    }
}


int main() {
    std::vector<Orderu> orders{
        {1234, "PAVEL", Order::CREATED},
        {2376, "VIKTOR",Order::PROCESSING},
        {6453, "ANDRE", Order::CREATED},
        {7235, "DANIL", Order::NONE},
    };
    printStatus(orders);
    std::cout << std::endl;
    auto filteredorders = filter(orders, Order::CREATED);
    printStatus(filteredorders);
    std::cout << std::endl;
    changeByIndex(orders, 1, Order::PAID);
    changeByIndex(orders, 2, Order::SHIPPED);
    printStatus(orders);
}
