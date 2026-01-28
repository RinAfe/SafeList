#include <iostream>
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
    std::cout << "   size(): " << list.size() << " (ожидается: 1)" << std::endl;
    std::cout << "   empty(): " << list.empty() << " (ожидается: 0)" << std::endl;

    list.push_back(20);
    std::cout << "   push_back(20)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 2)" << std::endl;

    // Тест 3: push_front
    std::cout << "\n3. Тест push_front:" << std::endl;
    list.push_front(5);
    std::cout << "   push_front(5)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 3)" << std::endl;

    list.push_front(1);
    std::cout << "   push_front(1)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 4)" << std::endl;

    // Тест 4: Итераторы (базовый обход)
    std::cout << "\n4. Тест итераторов (базовый обход):" << std::endl;
    std::cout << "   Элементы списка: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "(ожидается: 1 5 10 20)" << std::endl;

    // Тест 5: get_lock и передача lock в итератор
    std::cout << "\n5. Тест get_lock и передачи lock:" << std::endl;
    {
        auto lock = list.get_lock();
        auto it = list.begin(std::move(lock));
        auto end = list.end();

        std::cout << "   Элементы (с get_lock): ";
        while (it != end) {
            std::cout << *it << " ";
            ++it;
        }
        std::cout << "(ожидается: 1 5 10 20)" << std::endl;
    }

    // Тест 6: pop_front
    std::cout << "\n6. Тест pop_front:" << std::endl;
    auto front1 = list.pop_front();
    if (front1.has_value()) {
        std::cout << "   pop_front(): " << front1.value() << " (ожидается: 1)" << std::endl;
    }
    std::cout << "   size(): " << list.size() << " (ожидается: 3)" << std::endl;

    auto front2 = list.pop_front();
    if (front2.has_value()) {
        std::cout << "   pop_front(): " << front2.value() << " (ожидается: 5)" << std::endl;
    }
    std::cout << "   size(): " << list.size() << " (ожидается: 2)" << std::endl;

    std::cout << "   Элементы после двух pop_front: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "(ожидается: 10 20)" << std::endl;

    // Тест 7: pop_front из пустого списка
    std::cout << "\n7. Тест pop_front из пустого списка:" << std::endl;
    {
        ThreadSafeList<int> empty_list;
        auto front = empty_list.pop_front();
        if (!front.has_value()) {
            std::cout << "   pop_front() из пустого списка: nullopt (корректно)" << std::endl;
        }
    }

    // Тест 8: remove с итератором
    std::cout << "\n8. Тест remove с итератором:" << std::endl;

    // Добавляем еще элементов
    list.push_back(30);
    list.push_back(20); // Дубликат
    list.push_back(40);
    std::cout << "   Добавили 30, 20, 40" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 5)" << std::endl;

    std::cout << "   Список до удалений: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "(ожидается: 10 20 30 20 40)" << std::endl;

    // Удаляем первый элемент 20
    auto it1 = list.begin();
    bool removed1 = list.remove(20, it1);
    std::cout << "   remove(20, begin()): " << removed1 << " (ожидается: 1)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 4)" << std::endl;

    std::cout << "   Список после удаления первой 20: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "(ожидается: 10 30 20 40)" << std::endl;

    // Удаляем вторую 20, начиная с определенной позиции
    auto it2 = list.begin();
    ++it2; // Пропускаем 10
    bool removed2 = list.remove(20, it2);
    std::cout << "   remove(20, begin()+1): " << removed2 << " (ожидается: 1)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 3)" << std::endl;

    std::cout << "   Список после удаления второй 20: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "(ожидается: 10 30 40)" << std::endl;

    // Тест 9: remove несуществующего элемента
    std::cout << "\n9. Тест remove несуществующего элемента:" << std::endl;
    auto it3 = list.begin();
    bool removed3 = list.remove(99, it3);
    std::cout << "   remove(99, begin()): " << removed3 << " (ожидается: 0)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 3)" << std::endl;

    // Тест 10: remove с end() итератором
    std::cout << "\n10. Тест remove с end() итератором:" << std::endl;
    auto end_it = list.end();
    bool removed4 = list.remove(10, end_it);
    std::cout << "   remove(10, end()): " << removed4 << " (ожидается: 0)" << std::endl;

    // Тест 11: Конструктор по умолчанию итератора
    std::cout << "\n11. Тест конструктора по умолчанию итератора:" << std::endl;
    {
        typename ThreadSafeList<int>::Iterator default_it;
        std::cout << "   Создан default итератор" << std::endl;
        std::cout << "   getNode(): " << default_it.getNode() << " (ожидается: 0/nullptr)" << std::endl;
    }

    // Тест 12: clear
    std::cout << "\n12. Тест clear:" << std::endl;
    std::cout << "   size() до clear: " << list.size() << " (ожидается: 3)" << std::endl;
    list.clear();
    std::cout << "   size() после clear: " << list.size() << " (ожидается: 0)" << std::endl;
    std::cout << "   empty() после clear: " << list.empty() << " (ожидается: 1)" << std::endl;

    // Тест 13: Работа после clear
    std::cout << "\n13. Тест работы после clear:" << std::endl;
    list.push_back(100);
    list.push_front(50);
    std::cout << "   push_back(100), push_front(50)" << std::endl;
    std::cout << "   size(): " << list.size() << " (ожидается: 2)" << std::endl;

    std::cout << "   Элементы: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "(ожидается: 50 100)" << std::endl;

    // Тест 14: Проверка tail указателя
    std::cout << "\n14. Тест tail указателя (push_back/pop_front):" << std::endl;
    {
        ThreadSafeList<int> test_list;
        test_list.push_back(1);
        test_list.push_back(2);
        test_list.push_back(3);

        auto f1 = test_list.pop_front(); // удаляем 1
        auto f2 = test_list.pop_front(); // удаляем 2
        test_list.push_back(4); // добавляем в конец

        std::cout << "   Элементы: ";
        for (auto it = test_list.begin(); it != test_list.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "(ожидается: 3 4)" << std::endl;

        test_list.pop_front(); // удаляем 3
        test_list.pop_front(); // удаляем 4
        std::cout << "   После удаления всех элементов:" << std::endl;
        std::cout << "   empty(): " << test_list.empty() << " (ожидается: 1)" << std::endl;
        std::cout << "   size(): " << test_list.size() << " (ожидается: 0)" << std::endl;
    }

    // Тест 15: Многократное использование
    std::cout << "\n15. Тест многократного использования:" << std::endl;
    {
        ThreadSafeList<int> test_list;
        for (int i = 0; i < 5; ++i) {
            test_list.push_back(i * 10);
        }

        std::cout << "   Добавили 0, 10, 20, 30, 40" << std::endl;
        std::cout << "   Итерация 1: ";
        for (auto it = test_list.begin(); it != test_list.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << std::endl;

        // Удаляем средний элемент
        auto it = test_list.begin();
        ++it; ++it; // переходим на 20
        test_list.remove(*it, it);

        std::cout << "   Удалили 20" << std::endl;
        std::cout << "   Итерация 2: ";
        for (auto it = test_list.begin(); it != test_list.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "(ожидается: 0 10 30 40)" << std::endl;
    }

    std::cout << "\n=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ===" << std::endl;

    return 0;
}

