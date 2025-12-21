#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <locale>
#include <string>

using namespace std;

// Сортировка вставками
void insertion_sort(vector<int>& a) {
    size_t n = a.size();
    for (size_t i = 1; i < n; ++i) {
        int key = a[i];
        size_t j = i;
        while (j > 0 && a[j - 1] > key) {
            a[j] = a[j - 1];
            j--;
        }
        a[j] = key;
    }
}

// Сортировка Шелла
void shell_sort(vector<int>& a) {
    size_t n = a.size();
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

vector<int> make_random_data(size_t n) {
    static mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(-1000000, 1000000);

    vector<int> v(n);
    for (size_t i = 0; i < n; ++i)
        v[i] = dist(rng);

    return v;
}

// Отсортированный массив 
vector<int> make_sorted_data(size_t n) {
    vector<int> v = make_random_data(n);
    sort(v.begin(), v.end());
    return v;
}

// Обратно отсортированный массив 
vector<int> make_reverse_sorted_data(size_t n) {
    vector<int> v = make_random_data(n);
    sort(v.begin(), v.end(), greater<int>());
    return v;
}

int main() {
    setlocale(LC_ALL, "Russian");

    using clock = chrono::high_resolution_clock;
    using chrono::duration_cast;
    using chrono::microseconds;

    const vector<size_t> n_values = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
    const int num_runs = 500;

    enum class DataType { Random, Sorted, ReverseSorted };

    auto data_type_name = [](DataType t) -> string {
        switch (t) {
            case DataType::Random: return "Случайные данные";
            case DataType::Sorted: return "Отсортированные данные";
            case DataType::ReverseSorted: return "Обратные данные";
        }
        return "";
    };

    auto make_data_by_type = [](size_t n, DataType t) -> vector<int> {
        switch (t) {
            case DataType::Random: return make_random_data(n);
            case DataType::Sorted: return make_sorted_data(n);
            case DataType::ReverseSorted: return make_reverse_sorted_data(n);
        }
        return {};
    };

    const vector<DataType> types = { DataType::Random, DataType::Sorted, DataType::ReverseSorted };

    for (size_t n : n_values) {
        cout << "Размер массива n = " << n << "\n";

        for (DataType type : types) {
            double t_ins = 0.0;
            double t_shell = 0.0;
            double t_std = 0.0;

            for (int run = 0; run < num_runs; ++run) {
                vector<int> base = make_data_by_type(n, type);

                vector<int> v_ins = base;
                vector<int> v_shell = base;
                vector<int> v_std = base;

                auto s1 = clock::now();
                insertion_sort(v_ins);
                auto e1 = clock::now();
                t_ins += duration_cast<microseconds>(e1 - s1).count();

                auto s2 = clock::now();
                shell_sort(v_shell);
                auto e2 = clock::now();
                t_shell += duration_cast<microseconds>(e2 - s2).count();

                auto s3 = clock::now();
                sort(v_std.begin(), v_std.end());
                auto e3 = clock::now();
                t_std += duration_cast<microseconds>(e3 - s3).count();
            }

            cout << "\nТип данных: " << data_type_name(type) << "\n";
            cout << "  Среднее время (вставки)   : " << t_ins / num_runs << " мкс\n";
            cout << "  Среднее время (Шелл)      : " << t_shell / num_runs << " мкс\n";
            cout << "  Среднее время (std::sort) : " << t_std / num_runs << " мкс\n";
        }

        cout << "\n";
    }

    return 0;
}