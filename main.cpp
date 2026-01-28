#include <iostream>
#include <algorithm>
#include "ThreadSafeList.h"

int main() {
    std::cout << "=== ТЕСТИРОВАНИЕ ThreadSafeList ===" << std::endl;

    ThreadSafeList<int> list;

    // Тест 1: Конструктор и начальное состояние
    std::cout << "\n1. Тест конструктора и начального состояния:" << std::endl;
    std::cout << "   empty(): " << list.empty() << " (ожидается: 1)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 0)" << std::endl;

    // Тест 2: push_back
    std::cout << "\n2. Тест push_back:" << std::endl;
    list.push_back(10);
    std::cout << "   push_back(10)" << std::endl;
    list.push_back(20);
    std::cout << "   push_back(20)" << std::endl;
    list.push_back(30);
    std::cout << "   push_back(30)" << std::endl;

    // Тест 3: push_front
    std::cout << "\n3. Тест push_front:" << std::endl;
    list.push_front(5);
    std::cout << "   push_front(5)" << std::endl;
    list.push_front(1);
    std::cout << "   push_front(1)" << std::endl;

    std::cout << "   size(): " << list.size() << " (ожидается: 5)" << std::endl;

    // Тест 4: Итераторы (базовый обход)
    std::cout << "\n4. Тест итераторов (базовый обход):" << std::endl;
    std::cout << "   Элементы списка: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "(ожидается: 1 5 10 20 30)" << std::endl;

    // ТЕСТ 5: std::find (самое важное что ты просил)
    std::cout << "\n5. Тест std::find:" << std::endl;

    // Поиск существующего элемента
    auto it1 = std::find(list.begin(), list.end(), 10);
    if (it1 != list.end()) {
        std::cout << "   std::find(list.begin(), list.end(), 10): НАЙДЕНО " << *it1
                  << " (ожидается: 10)" << std::endl;
    } else {
        std::cout << "   std::find(list.begin(), list.end(), 10): НЕ НАЙДЕНО (неправильно!)" << std::endl;
    }

    // Поиск другого существующего элемента
    auto it2 = std::find(list.begin(), list.end(), 20);
    if (it2 != list.end()) {
        std::cout << "   std::find(list.begin(), list.end(), 20): НАЙДЕНО " << *it2
                  << " (ожидается: 20)" << std::endl;
    } else {
        std::cout << "   std::find(list.begin(), list.end(), 20): НЕ НАЙДЕНО (неправильно!)" << std::endl;
    }

    // Поиск несуществующего элемента
    auto it3 = std::find(list.begin(), list.end(), 99);
    if (it3 != list.end()) {
        std::cout << "   std::find(list.begin(), list.end(), 99): НАЙДЕНО " << *it3
                  << " (неправильно, не должно быть!)" << std::endl;
    } else {
        std::cout << "   std::find(list.begin(), list.end(), 99): НЕ НАЙДЕНО (правильно!)" << std::endl;
    }

    // Поиск первого элемента
    auto it4 = std::find(list.begin(), list.end(), 1);
    if (it4 != list.end()) {
        std::cout << "   std::find(list.begin(), list.end(), 1): НАЙДЕНО " << *it4
                  << " (ожидается: 1)" << std::endl;
    }

    // Поиск последнего элемента
    auto it5 = std::find(list.begin(), list.end(), 30);
    if (it5 != list.end()) {
        std::cout << "   std::find(list.begin(), list.end(), 30): НАЙДЕНО " << *it5
                  << " (ожидается: 30)" << std::endl;
    }

    // Тест 6: std::find_if
    std::cout << "\n6. Тест std::find_if:" << std::endl;

    // Поиск первого четного числа
    auto even_it = std::find_if(list.begin(), list.end(), [](int x) {
        return x % 2 == 0;
    });
    if (even_it != list.end()) {
        std::cout << "   Первое четное число: " << *even_it
                  << " (ожидается: 10 или другое четное)" << std::endl;
    }

    // Поиск числа больше 25
    auto greater_it = std::find_if(list.begin(), list.end(), [](int x) {
        return x > 25;
    });
    if (greater_it != list.end()) {
        std::cout << "   Первое число > 25: " << *greater_it
                  << " (ожидается: 30)" << std::endl;
    }

    // Тест 7: Использование найденного итератора
    std::cout << "\n7. Тест использования найденного итератора:" << std::endl;
    auto found_it = std::find(list.begin(), list.end(), 5);
    if (found_it != list.end()) {
        std::cout << "   Нашли элемент 5" << std::endl;
        std::cout << "   Значение через *: " << *found_it << " (ожидается: 5)" << std::endl;

        // Можем использовать для удаления
        bool removed = list.remove(*found_it, found_it);
        std::cout << "   Удалили элемент 5: " << removed << " (ожидается: 1)" << std::endl;
        std::cout << "   Размер после удаления: " << list.size() << " (ожидается: 4)" << std::endl;

        std::cout << "   Новый список: ";
        for (auto it = list.begin(); it != list.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "(ожидается: 1 10 20 30)" << std::endl;
    }

    // Тест 8: std::count
    std::cout << "\n8. Тест std::count:" << std::endl;

    // Добавим дубликаты
    list.push_back(20);
    list.push_back(20);
    std::cout << "   Добавили две 20" << std::endl;

    int count_20 = std::count(list.begin(), list.end(), 20);
    std::cout << "   std::count(20): " << count_20 << " (ожидается: 3)" << std::endl;

    int count_99 = std::count(list.begin(), list.end(), 99);
    std::cout << "   std::count(99): " << count_99 << " (ожидается: 0)" << std::endl;

    // Тест 9: std::for_each
    std::cout << "\n9. Тест std::for_each:" << std::endl;
    std::cout << "   Элементы через std::for_each: ";
    std::for_each(list.begin(), list.end(), [](int x) {
        std::cout << x << " ";
    });
    std::cout << std::endl;

    // Тест 10: Проверка, что после всех операций find все еще работает
    std::cout << "\n10. Финальная проверка std::find:" << std::endl;

    // Ищем все элементы по очереди
    int test_values[] = {1, 10, 20, 30};
    for (int val : test_values) {
        auto it = std::find(list.begin(), list.end(), val);
        if (it != list.end()) {
            std::cout << "   " << val << " найден ✓" << std::endl;
        } else {
            std::cout << "   " << val << " НЕ найден ✗" << std::endl;
        }
    }

    // Ищем несуществующее
    auto not_found_it = std::find(list.begin(), list.end(), 999);
    if (not_found_it == list.end()) {
        std::cout << "   999 не найден (правильно) ✓" << std::endl;
    }

    std::cout << "\n=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ===" << std::endl;

    return 0;
}