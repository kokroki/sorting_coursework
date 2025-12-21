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
    for (size_t i = 1; i < n; ++i) {                  // внешний цикл: расширяем отсортированную часть на 1 элемент
        int key = a[i];                               // элемент, который нужно вставить
        size_t j = i;                                 // индекс для сдвига элементов вправо
        while (j > 0 && a[j - 1] > key) {             // пока слева есть элементы больше key, сдвигаем их вправо
            a[j] = a[j - 1];
            --j;
        }
        a[j] = key;                                   // фиксируем key на найденной позиции
    }
}

// Сортировка Шелла
void shell_sort(vector<int>& a) {
    size_t n = a.size();
    for (size_t gap = n / 2; gap > 0; gap /= 2) {     // выбор шага: n/2, затем n/4, ... , 1
        for (size_t i = gap; i < n; ++i) {            // проход по элементам, начиная с gap
            int temp = a[i];                           // текущий элемент для вставки в свою подпоследовательность
            size_t j = i;
            while (j >= gap && a[j - gap] > temp) {   // сдвигаем элементы подпоследовательности на gap вправо
                a[j] = a[j - gap];
                j -= gap;
            }
            a[j] = temp;                               // устанавливаем temp на корректное место
        }
    }
}

// Генерация случайного массива заданного размера
vector<int> make_random_data(size_t n) {
    static mt19937 rng(random_device{}());            // генератор инициализируется один раз на всю программу
    uniform_int_distribution<int> dist(-1000000, 1000000);

    vector<int> v(n);
    for (size_t i = 0; i < n; ++i) {
        v[i] = dist(rng);                             // заполнение массива случайными значениями
    }
    return v;
}

// Формирование отсортированного массива 
vector<int> make_sorted_data(size_t n) {
    vector<int> v = make_random_data(n);              // генерируем случайные данные
    sort(v.begin(), v.end());                         // сортируем по возрастанию
    return v;
}

// Формирование обратно отсортированного массива 
vector<int> make_reverse_sorted_data(size_t n) {
    vector<int> v = make_random_data(n);
    sort(v.begin(), v.end(), greater<int>());         // сортируем по убыванию
    return v;
}

enum class DataType { Random, Sorted, ReverseSorted };

// Возвращает строковое представление типа данных
string data_type_name(DataType t) {
    switch (t) {
        case DataType::Random: return "Случайные данные";
        case DataType::Sorted: return "Отсортированные данные";
        case DataType::ReverseSorted: return "Обратные данные";
    }
    return "";
}

// Генерация данных в зависимости от типа
vector<int> make_data_by_type(size_t n, DataType t) {
    switch (t) {
        case DataType::Random: return make_random_data(n);
        case DataType::Sorted: return make_sorted_data(n);
        case DataType::ReverseSorted: return make_reverse_sorted_data(n);
    }
    return {};
}
int main() {
    setlocale(LC_ALL, "Russian");

    using clock = chrono::high_resolution_clock;      // высокоточный таймер
    using chrono::duration_cast;
    using chrono::microseconds;

    const vector<size_t> n_values = {10, 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
    const int num_runs = 500;                          // число прогонов для усреднения времени

    const vector<DataType> types = { DataType::Random, DataType::Sorted, DataType::ReverseSorted };

    for (size_t n : n_values) {                        // цикл по размерам массивов
        cout << "Размер массива n = " << n << "\n";

        for (DataType type : types) {                  // цикл по типам входных данных
            double t_ins = 0.0;
            double t_shell = 0.0;
            double t_std = 0.0;

            for (int run = 0; run < num_runs; ++run) { // повторяем замеры, чтобы усреднить результат
                vector<int> base = make_data_by_type(n, type);

                vector<int> v_ins = base;              // отдельная копия под insertion sort
                vector<int> v_shell = base;            // отдельная копия под shell sort
                vector<int> v_std = base;              // отдельная копия под std::sort

                auto s1 = clock::now();                // старт таймера для insertion sort
                insertion_sort(v_ins);
                auto e1 = clock::now();                // конец таймера
                t_ins += duration_cast<microseconds>(e1 - s1).count();

                auto s2 = clock::now();                // старт таймера для shell sort
                shell_sort(v_shell);
                auto e2 = clock::now();
                t_shell += duration_cast<microseconds>(e2 - s2).count();

                auto s3 = clock::now();                // старт таймера для std::sort
                sort(v_std.begin(), v_std.end());
                auto e3 = clock::now();
                t_std += duration_cast<microseconds>(e3 - s3).count();
            }

            cout << "\nТип данных: " << data_type_name(type) << "\n";
            cout << "  Среднее время (вставки)   : " << t_ins / num_runs << " мкс\n";   // усреднение по прогонам
            cout << "  Среднее время (Шелл)      : " << t_shell / num_runs << " мкс\n";
            cout << "  Среднее время (std::sort) : " << t_std / num_runs << " мкс\n";
        }

        cout << "\n";
    }

    return 0;
}