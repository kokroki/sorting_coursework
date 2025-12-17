#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <locale>
#include <string>

using namespace std;

// -------------------- Сортировки --------------------

void insertion_sort(vector<int>& a) {
    const size_t n = a.size();
    for (size_t i = 1; i < n; ++i) {
        int key = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;
    }
}

void shell_sort(vector<int>& a) {
    const size_t n = a.size();
    for (size_t gap = n / 2; gap > 0; gap /= 2) {
        for (size_t i = gap; i < n; ++i) {
            int temp = a[i];
            size_t j = i;
            while (j >= gap && a[j - gap] > temp) {
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;
        }
    }
}

// -------------------- Генерация данных --------------------

static mt19937& rng() {
    static mt19937 gen(random_device{}());
    return gen;
}

vector<int> make_random_data(size_t n) {
    uniform_int_distribution<int> dist(-1'000'000, 1'000'000);
    vector<int> v(n);
    for (size_t i = 0; i < n; ++i) v[i] = dist(rng());
    return v;
}

vector<int> make_sorted_data(size_t n) {
    vector<int> v = make_random_data(n);
    sort(v.begin(), v.end());
    return v;
}

vector<int> make_reverse_sorted_data(size_t n) {
    vector<int> v = make_random_data(n);
    sort(v.begin(), v.end(), greater<int>());
    return v;
}

enum class DataType { Random, Sorted, ReverseSorted };

string data_type_name(DataType t) {
    switch (t) {
        case DataType::Random:        return "Случайные данные";
        case DataType::Sorted:        return "Отсортированные данные";
        case DataType::ReverseSorted: return "Обратные данные";
    }
    return "";
}

vector<int> make_data(size_t n, DataType t) {
    switch (t) {
        case DataType::Random:        return make_random_data(n);
        case DataType::Sorted:        return make_sorted_data(n);
        case DataType::ReverseSorted: return make_reverse_sorted_data(n);
    }
    return {};
}

// -------------------- Бенчмарк --------------------

using clock_type = chrono::high_resolution_clock;

template <class F>
double measure_us(F&& fn) {
    using chrono::duration_cast;
    using chrono::microseconds;

    const auto start = clock_type::now();
    fn();
    const auto end = clock_type::now();
    return (double)duration_cast<microseconds>(end - start).count();
}

struct AvgTimes {
    double ins = 0.0;
    double shell = 0.0;
    double stl = 0.0;
};

AvgTimes benchmark_once(const vector<int>& base) {
    AvgTimes t;

    vector<int> v_ins = base;
    vector<int> v_shell = base;
    vector<int> v_std = base;

    t.ins = measure_us([&] { insertion_sort(v_ins); });
    t.shell = measure_us([&] { shell_sort(v_shell); });
    t.stl = measure_us([&] { sort(v_std.begin(), v_std.end()); });

    return t;
}

AvgTimes benchmark_average(size_t n, DataType type, int num_runs) {
    AvgTimes sum;

    for (int run = 0; run < num_runs; ++run) {
        vector<int> base = make_data(n, type);
        AvgTimes one = benchmark_once(base);

        sum.ins += one.ins;
        sum.shell += one.shell;
        sum.stl += one.stl;
    }

    sum.ins /= num_runs;
    sum.shell /= num_runs;
    sum.stl /= num_runs;
    return sum;
}

// -------------------- main --------------------

int main() {
    setlocale(LC_ALL, "Russian");

    const vector<size_t> n_values = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
    const int num_runs = 500;

    const vector<DataType> types = {
        DataType::Random, DataType::Sorted, DataType::ReverseSorted
    };

    for (size_t n : n_values) {
        cout << "Размер массива n = " << n << "\n";

        for (DataType type : types) {
            AvgTimes avg = benchmark_average(n, type, num_runs);

            cout << "\nТип данных: " << data_type_name(type) << "\n";
            cout << "  Среднее время (вставки)   : " << avg.ins << " мкс\n";
            cout << "  Среднее время (Шелл)      : " << avg.shell << " мкс\n";
            cout << "  Среднее время (std::sort) : " << avg.stl << " мкс\n";
        }

        cout << "\n";
    }

    return 0;
}