#include <benchmark/benchmark.h>
#include <vector>
#include <deque>
#include <list>
#include <algorithm>
#include <random>
#include <numeric>

enum class ContainerType { Vector, Deque, List };
enum class Operation {
    PushBack, PushFront, RandomInsert,
    RandomErase, IterateSum, RandomAccess
};
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

// PushBack benchmark
template<class Seq>
static void BM_PushBack(benchmark::State& st) {
    int N = static_cast<int>(st.range(0));
    std::vector<int> payload(N);
    std::mt19937 gen(12345);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (int &x : payload) x = dist(gen);

    for (auto _ : st) {
        st.PauseTiming();
        Seq container;
        st.ResumeTiming();

        for (int x : payload) {
            container.push_back(x);
        }

        benchmark::DoNotOptimize(container.size());
        benchmark::ClobberMemory();
    }
}

// PushFront benchmark
template<class Seq>
static void BM_PushFront(benchmark::State& st) {
    int N = static_cast<int>(st.range(0));
    std::vector<int> payload(N);
    std::mt19937 gen(12345);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (int &x : payload) x = dist(gen);

    for (auto _ : st) {
        st.PauseTiming();
        Seq container;
        st.ResumeTiming();

        for (int x : payload) {
            container.push_front(x);
        }

        benchmark::DoNotOptimize(container.size());
        benchmark::ClobberMemory();
    }
}

// RandomInsert benchmark
template<class Seq>
static void BM_RandomInsert(benchmark::State& st) {
    int container_size = static_cast<int>(st.range(0));
    int insert_count = static_cast<int>(st.range(1));

    std::vector<int> payload(insert_count);
    std::mt19937 gen(12345);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (int &x : payload) x = dist(gen);

    std::vector<size_t> positions(insert_count);
    std::uniform_int_distribution<size_t> pos_dist(0, container_size + insert_count - 1);
    for (size_t &pos : positions) pos = pos_dist(gen);

    for (auto _ : st) {
        st.PauseTiming();
        Seq container(container_size, 0); // Pre-fill container
        st.ResumeTiming();

        auto it = container.begin();
        for (size_t i = 0; i < insert_count; ++i) {
            std::advance(it, positions[i] % (container.size() + 1));
            it = container.insert(it, payload[i]);
        }

        benchmark::DoNotOptimize(container.size());
        benchmark::ClobberMemory();
    }
}

// RandomErase benchmark
template<class Seq>
static void BM_RandomErase(benchmark::State& st) {
    int container_size = static_cast<int>(st.range(0));
    int erase_count = static_cast<int>(st.range(1));

    std::mt19937 gen(12345);
    std::uniform_int_distribution<size_t> pos_dist(0, container_size - 1);

    std::vector<size_t> positions(erase_count);
    for (size_t &pos : positions) pos = pos_dist(gen);

    for (auto _ : st) {
        st.PauseTiming();
        Seq container(container_size, 0); // Pre-fill container
        st.ResumeTiming();

        for (size_t i = 0; i < erase_count; ++i) {
            auto it = container.begin();
            std::advance(it, positions[i] % container.size());
            container.erase(it);
        }

        benchmark::DoNotOptimize(container.size());
        benchmark::ClobberMemory();
    }
}

// IterateSum benchmark
template<class Seq>
static void BM_IterateSum(benchmark::State& st) {
    int N = static_cast<int>(st.range(0));
    std::vector<int> payload(N);
    std::mt19937 gen(12345);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (int &x : payload) x = dist(gen);

    st.PauseTiming();
    Seq container(payload.begin(), payload.end());
    st.ResumeTiming();

    for (auto _ : st) {
        long long sum = 0;
        for (const auto& x : container) {
            sum += x;
        }

        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
}

// RandomAccess benchmark
template<class Seq>
static void BM_RandomAccess(benchmark::State& st) {
    int container_size = static_cast<int>(st.range(0));
    int access_count = static_cast<int>(st.range(1));

    std::vector<int> payload(container_size);
    std::mt19937 gen(12345);
    std::uniform_int_distribution<int> dist(0, 1000000);
    for (int &x : payload) x = dist(gen);

    std::vector<size_t> positions(access_count);
    std::uniform_int_distribution<size_t> pos_dist(0, container_size - 1);
    for (size_t &pos : positions) pos = pos_dist(gen);

    st.PauseTiming();
    Seq container(payload.begin(), payload.end());
    st.ResumeTiming();

    for (auto _ : st) {
        long long sum = 0;
        for (size_t pos : positions) {
            if constexpr (std::is_same_v<Seq, std::list<int>>) {
                auto it = container.begin();
                std::advance(it, pos);
                sum += *it;
            } else {
                sum += container[pos];
            }
        }

        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
}

// Registration helpers
#define REG_ALL_OPS(Seq, name) \
    BENCHMARK_TEMPLATE(BM_PushBack, Seq)->Name(name "/PushBack")->Range(8<<10, 8<<17); \
    BENCHMARK_TEMPLATE(BM_PushFront, Seq)->Name(name "/PushFront")->Range(8<<10, 8<<17); \
    BENCHMARK_TEMPLATE(BM_RandomInsert, Seq)->Name(name "/RandomInsert")->ArgsProduct({{8<<12, 8<<14, 8<<16}, {1024, 4096}}); \
    BENCHMARK_TEMPLATE(BM_RandomErase, Seq)->Name(name "/RandomErase")->ArgsProduct({{8<<12, 8<<14, 8<<16}, {1024, 4096}}); \
    BENCHMARK_TEMPLATE(BM_IterateSum, Seq)->Name(name "/IterateSum")->Range(8<<10, 8<<20); \
    BENCHMARK_TEMPLATE(BM_RandomAccess, Seq)->Name(name "/RandomAccess")->ArgsProduct({{8<<12, 8<<14, 8<<16}, {4096, 65536}});

REG_ALL_OPS(std::vector<int>, "vector")
REG_ALL_OPS(std::deque<int>, "deque")
REG_ALL_OPS(std::list<int>, "list")

BENCHMARK_MAIN();