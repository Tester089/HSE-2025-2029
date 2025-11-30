#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <functional>
#include <thread>

enum class ContainerType { Vector, Deque, List };
enum class Operation { PushBack, PushFront, RandomInsert, RandomErase, IterateSum, RandomAccess };
enum class TimeUnit { Milliseconds, Seconds };
union TimeValue {
    double ms;
    double sec;
};
struct TimeResult {
    ContainerType ct;
    Operation op;
    TimeUnit unit;
    TimeValue val;
};

template<typename Container>
void perform_push_back(Container& container, const std::vector<int>& data) {
    for (int value : data) {
        container.push_back(value);
    }
}

template<typename Container>
void perform_push_front(Container& container, const std::vector<int>& data) {
    if constexpr (std::is_same_v<Container, std::vector<int>>) {
        for (int value : data) {
            container.insert(container.begin(), value);
        }
    } else {
        for (int value : data) {
            container.push_front(value);
        }
    }
}

template<typename Container>
void perform_random_insert(Container& container, const std::vector<int>& data, int operations) {
    std::mt19937 gen(12345);
    std::uniform_int_distribution<size_t> pos_dist(0, container.size());

    for (int i = 0; i < operations && i < static_cast<int>(data.size()); ++i) {
        size_t pos = pos_dist(gen);
        auto it = container.begin();
        std::advance(it, pos);
        container.insert(it, data[i]);
    }
}

template<typename Container>
void perform_random_erase(Container& container, int operations) {
    std::mt19937 gen(12345);

    for (int i = 0; i < operations && !container.empty(); ++i) {
        std::uniform_int_distribution<size_t> pos_dist(0, container.size() - 1);
        size_t pos = pos_dist(gen);
        auto it = container.begin();
        std::advance(it, pos);
        container.erase(it);
    }
}

template<typename Container>
long long perform_iterate_sum(const Container& container) {
    long long sum = 0;
    for (const auto& value : container) {
        sum += value;
    }
    return sum;
}

template<typename Container>
long long perform_random_access(Container& container, int operations) {
    std::mt19937 gen(12345);
    std::uniform_int_distribution<size_t> idx_dist(0, container.size() - 1);

    long long sum = 0;
    for (int i = 0; i < operations; ++i) {
        size_t idx = idx_dist(gen);
        if constexpr (std::is_same_v<Container, std::vector<int>>) {
            sum += container[idx];
        } else {
            auto it = container.begin();
            std::advance(it, idx);
            sum += *it;
        }
    }
    return sum;
}

template<typename Container>
double measure_operation(std::function<void(Container&)> operation_func,
                         int repeats = 50) {
    double total_time = 0.0;

    for (int i = 0; i < repeats; ++i) {
        Container container;
        auto start = std::chrono::high_resolution_clock::now();
        operation_func(container);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::milli> elapsed = end - start;
        total_time += elapsed.count();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    return total_time / repeats;
}

int main() {
    std::ofstream results_file("result.txt");
    if (!results_file.is_open()) {
        std::cerr << "Could not open result file" << std::endl;
        return 1;
    }

    std::vector<int> sizes = {1024, 4096, 10000};
    int repeats = 50;
    std::mt19937 gen(12345);

    results_file << "Homework #1: Performance measurement of STL containers\n";
    results_file << "=======================================================\n\n";

    for (int size : sizes) {
        std::vector<int> data(size);
        std::uniform_int_distribution<int> dist(0, 1000000);
        for (int& val : data) {
            val = dist(gen);
        }

        results_file << "N = " << size << " elements\n";
        results_file << "-------------------\n";

        {
            auto operation = [&](std::vector<int>& container) {
                container.clear();
                perform_push_back(container, data);
            };
            double time_ms = measure_operation<std::vector<int>>(operation, repeats);
            results_file << "vector PushBack: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        {
            auto operation = [&](std::deque<int>& container) {
                container.clear();
                perform_push_back(container, data);
            };
            double time_ms = measure_operation<std::deque<int>>(operation, repeats);
            results_file << "deque PushBack: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        {
            auto operation = [&](std::list<int>& container) {
                container.clear();
                perform_push_back(container, data);
            };
            double time_ms = measure_operation<std::list<int>>(operation, repeats);
            results_file << "list PushBack: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        results_file << "\n";

        {
            auto operation = [&](std::vector<int>& container) {
                container.clear();
                perform_push_front(container, data);
            };
            double time_ms = measure_operation<std::vector<int>>(operation, repeats);
            results_file << "vector PushFront: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        {
            auto operation = [&](std::deque<int>& container) {
                container.clear();
                perform_push_front(container, data);
            };
            double time_ms = measure_operation<std::deque<int>>(operation, repeats);
            results_file << "deque PushFront: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        {
            auto operation = [&](std::list<int>& container) {
                container.clear();
                perform_push_front(container, data);
            };
            double time_ms = measure_operation<std::list<int>>(operation, repeats);
            results_file << "list PushFront: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        results_file << "\n";

        {
            int operations = 256;
            auto operation = [&](std::vector<int>& container) {
                container.clear();
                for (int i = 0; i < size / 2; ++i) {
                    container.push_back(i);
                }
                perform_random_insert(container, data, operations);
            };
            double time_ms = measure_operation<std::vector<int>>(operation, repeats);
            results_file << "vector RandomInsert (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        {
            int operations = 256;
            auto operation = [&](std::deque<int>& container) {
                container.clear();
                for (int i = 0; i < size / 2; ++i) {
                    container.push_back(i);
                }
                perform_random_insert(container, data, operations);
            };
            double time_ms = measure_operation<std::deque<int>>(operation, repeats);
            results_file << "deque RandomInsert (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        {
            int operations = 256;
            auto operation = [&](std::list<int>& container) {
                container.clear();
                for (int i = 0; i < size / 2; ++i) {
                    container.push_back(i);
                }
                perform_random_insert(container, data, operations);
            };
            double time_ms = measure_operation<std::list<int>>(operation, repeats);
            results_file << "list RandomInsert (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        results_file << "\n";

        {
            int operations = 256;
            auto operation = [&](std::vector<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                perform_random_erase(container, operations);
            };
            double time_ms = measure_operation<std::vector<int>>(operation, repeats);
            results_file << "vector RandomErase (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        {
            int operations = 256;
            auto operation = [&](std::deque<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                perform_random_erase(container, operations);
            };
            double time_ms = measure_operation<std::deque<int>>(operation, repeats);
            results_file << "deque RandomErase (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        {
            int operations = 256;
            auto operation = [&](std::list<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                perform_random_erase(container, operations);
            };
            double time_ms = measure_operation<std::list<int>>(operation, repeats);
            results_file << "list RandomErase (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        results_file << "\n";

        {
            auto operation = [&](std::vector<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                volatile long long result = perform_iterate_sum(container);
                (void)result;
            };
            double time_ms = measure_operation<std::vector<int>>(operation, repeats);
            results_file << "vector IterateSum: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        {
            auto operation = [&](std::deque<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                volatile long long result = perform_iterate_sum(container);
                (void)result;
            };
            double time_ms = measure_operation<std::deque<int>>(operation, repeats);
            results_file << "deque IterateSum: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        {
            auto operation = [&](std::list<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                volatile long long result = perform_iterate_sum(container);
                (void)result;
            };
            double time_ms = measure_operation<std::list<int>>(operation, repeats);
            results_file << "list IterateSum: " << std::fixed << std::setprecision(4)
                         << time_ms << " ms\n";
        }

        results_file << "\n";

        {
            int operations = 1024;
            auto operation = [&](std::vector<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                volatile long long result = perform_random_access(container, operations);
                (void)result;
            };
            double time_ms = measure_operation<std::vector<int>>(operation, repeats);
            results_file << "vector RandomAccess (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        {
            int operations = 1024;
            auto operation = [&](std::deque<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                volatile long long result = perform_random_access(container, operations);
                (void)result;
            };
            double time_ms = measure_operation<std::deque<int>>(operation, repeats);
            results_file << "deque RandomAccess (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        {
            int operations = 1024;
            auto operation = [&](std::list<int>& container) {
                container.clear();
                for (int i = 0; i < size; ++i) {
                    container.push_back(i);
                }
                volatile long long result = perform_random_access(container, operations);
                (void)result;
            };
            double time_ms = measure_operation<std::list<int>>(operation, repeats);
            results_file << "list RandomAccess (" << operations << " ops): "
                         << std::fixed << std::setprecision(4) << time_ms << " ms\n";
        }

        results_file << "\n";
    }

    results_file.close();

    return 0;
}