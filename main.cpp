#include <iostream>
#include <cassert>
#include <thread>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include "ThreadSafeList.h"

void test_basic_operations() {
    std::cout << "Тестирование базовых операций..." << std::endl;
    ThreadSafeList<int> list;

    assert(list.empty());
    assert(list.size() == 0);

    list.push_back(1);
    assert(!list.empty());
    assert(list.size() == 1);

    list.push_front(2);
    assert(list.size() == 2);

    auto front = list.pop_front();
    assert(front.has_value());
    assert(front.value() == 2);
    assert(list.size() == 1);

    list.push_back(3);
    list.push_back(4);
    assert(list.size() == 3);

    list.clear();
    assert(list.empty());
    assert(list.size() == 0);

    std::cout << "Базовые операции прошли тест!" << std::endl << std::endl;
}

void test_iterator_operations() {
    std::cout << "Тестирование операций с итераторами..." << std::endl;
    ThreadSafeList<int> list;

    for (int i = 0; i < 5; ++i) {
        list.push_back(i);
    }

    int expected = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        assert(*it == expected);
        ++expected;
    }
    assert(expected == 5);

    auto it = list.begin();
    ++it;
    ++it;
    assert(*it == 2);

    std::cout << "Операции с итераторами прошли тест!" << std::endl << std::endl;
}

void test_erase_operations() {
    std::cout << "Тестирование операций удаления..." << std::endl;
    ThreadSafeList<int> list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(2);
    list.push_back(4);

    assert(list.size() == 5);

    bool removed = list.erase(2);
    assert(removed);
    assert(list.size() == 4);

    auto it = list.begin();
    ++it;
    assert(*it == 3);

    removed = list.erase(10);
    assert(!removed);

    list.clear();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.push_back(4);
    list.push_back(5);

    auto first = list.begin();
    ++first;
    auto last = list.begin();
    for (int i = 0; i < 4; ++i) ++last;

    removed = list.remove(3, first, last);
    assert(removed);
    assert(list.size() == 4);

    int sum = 0;
    for (auto val : list) {
        sum += val;
    }
    assert(sum == 12);

    removed = list.remove(1);
    assert(removed);
    assert(list.size() == 3);

    std::cout << "Операции удаления прошли тест!" << std::endl << std::endl;
}

void test_concurrent_access() {
    std::cout << "Тестирование конкурентного доступа..." << std::endl;
    ThreadSafeList<int> list;
    const int NUM_THREADS = 10;
    const int OPERATIONS_PER_THREAD = 1000;

    std::vector<std::thread> threads;
    std::atomic<int> total_push_count{0};
    std::atomic<int> total_pop_count{0};

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&list, i, OPERATIONS_PER_THREAD, &total_push_count, &total_pop_count]() {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 100);

            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                int operation = dis(gen) % 3;

                switch (operation) {
                    case 0:
                        list.push_front(j);
                        total_push_count++;
                        break;
                    case 1:
                        list.push_back(j);
                        total_push_count++;
                        break;
                    case 2:
                        if (list.pop_front().has_value()) {
                            total_pop_count++;
                        }
                        break;
                }

                if (j % 100 == 0) {
                    list.size();
                }
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    int final_size = list.size();
    int expected_size = total_push_count - total_pop_count;

    std::cout << "Финальный размер списка: " << final_size << std::endl;
    std::cout << "Ожидаемый размер: " << expected_size << std::endl;
    std::cout << "Всего добавлений: " << total_push_count << std::endl;
    std::cout << "Всего удалений: " << total_pop_count << std::endl;
    std::cout << "Время выполнения: " << duration.count() << " мс" << std::endl;

    assert(final_size == expected_size);

    std::cout << "Конкурентный доступ прошел тест!" << std::endl << std::endl;
}

void test_boundary_cases() {
    std::cout << "Тестирование граничных случаев..." << std::endl;
    ThreadSafeList<int> list;

    auto empty_pop = list.pop_front();
    assert(!empty_pop.has_value());

    assert(list.empty());
    assert(list.size() == 0);

    list.push_back(1);
    list.push_back(2);

    list.erase(1);
    assert(list.size() == 1);

    list.erase(2);
    assert(list.empty());

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    auto it = list.begin();
    auto next_it = list.erase(it);
    assert(*next_it == 2);
    assert(list.size() == 2);

    list.clear();
    for (int i = 0; i < 100; ++i) {
        list.push_back(i);
    }
    assert(list.size() == 100);

    std::cout << "Граничные случаи прошли тест!" << std::endl << std::endl;
}

void test_concurrent_erase() {
    std::cout << "Тестирование конкурентного удаления..." << std::endl;
    ThreadSafeList<int> list;
    const int NUM_ELEMENTS = 10000;
    const int NUM_THREADS = 8;

    for (int i = 0; i < NUM_ELEMENTS; ++i) {
        list.push_back(i % 10);
    }

    std::vector<std::thread> threads;
    std::atomic<int> total_removed{0};

    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&list, t, &total_removed]() {
            for (int i = 0; i < 100; ++i) {
                if (list.erase(t % 10)) {
                    total_removed++;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Удалено элементов: " << total_removed << std::endl;
    std::cout << "Финальный размер: " << list.size() << std::endl;

    assert(list.size() == NUM_ELEMENTS - total_removed);

    std::cout << "Конкурентное удаление прошло тест!" << std::endl << std::endl;
}

void test_iteration_while_modifying() {
    std::cout << "Тестирование итерации во время модификации..." << std::endl;
    ThreadSafeList<int> list;

    for (int i = 0; i < 10; ++i) {
        list.push_back(i);
    }

    std::thread modifier([&list]() {
        for (int i = 0; i < 5; ++i) {
            list.push_back(100 + i);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            list.erase(2 + i);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    int iteration_count = 0;
    for (auto it = list.begin(); it != list.end(); ++it) {
        iteration_count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    modifier.join();

    std::cout << "Количество итераций: " << iteration_count << std::endl;
    std::cout << "Финальный размер: " << list.size() << std::endl;

    std::cout << "Итерация во время модификации прошла тест!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== Начало тестирования ThreadSafeList ===" << std::endl << std::endl;

    try {
        test_basic_operations();
        test_iterator_operations();
        test_erase_operations();
        test_boundary_cases();
        test_concurrent_access();
        test_concurrent_erase();
        test_iteration_while_modifying();

        std::cout << "=== Все тесты успешно пройдены! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка во время тестирования: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Неизвестная ошибка во время тестирования!" << std::endl;
        return 1;
    }
}