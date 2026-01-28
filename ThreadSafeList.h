#pragma once

#include <mutex>
#include <memory>
#include <optional>

template <typename T>
class ThreadSafeList {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;

        Node() : next(nullptr) {}
        explicit Node(T _data) : data(std::make_shared<T>(std::move(_data))), next(nullptr) {}
    };

    Node head;
    Node* tail;
    mutable std::recursive_mutex mutex;
    std::size_t size_count = 0;

public:
    class Iterator {
    private:
        Node* current;
        std::unique_lock<std::recursive_mutex> lock;

    public:
        Iterator() : current(nullptr) {}

        Iterator(Node* node, std::unique_lock<std::recursive_mutex>&& lock) noexcept
            : current(node), lock(std::move(lock)) {}

        // Старый конструктор для обратной совместимости
        Iterator(Node* node, std::recursive_mutex& mtx, bool already_locked = false)
            : current(node) {
            if (!already_locked) {
                lock = std::unique_lock<std::recursive_mutex>(mtx);
            }
        }

        Iterator(const Iterator& other) : current(other.current) {
            if (current != nullptr) {
                lock = std::unique_lock<std::recursive_mutex>(*other.lock.mutex());
            }
        }

        Iterator& operator=(const Iterator& other) {
            if (this != &other) {
                current = other.current;
                if (current != nullptr) {
                    lock = std::unique_lock<std::recursive_mutex>(*other.lock.mutex());
                } else {
                    lock.unlock();
                }
            }
            return *this;
        }

        Iterator& operator++() {
            if (current) current = current->next.get();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }

        T& operator*() {
            return *(current->data);
        }

        Node* getNode() const { return current; }
    };

    ThreadSafeList() : tail(&head) {}
    ~ThreadSafeList() = default;

    ThreadSafeList(const ThreadSafeList&) = delete;
    ThreadSafeList& operator=(const ThreadSafeList&) = delete;

    void push_front(T _data) {
        std::unique_ptr<Node> temp = std::make_unique<Node>(std::move(_data));
        std::lock_guard<std::recursive_mutex> lock(mutex);

        temp->next = std::move(head.next);
        head.next = std::move(temp);

        if (tail == &head) {
            tail = head.next.get();
        }
        size_count++;
    }

    void push_back(T _data) {
        std::unique_ptr<Node> temp = std::make_unique<Node>(std::move(_data));
        std::lock_guard<std::recursive_mutex> lock(mutex);

        tail->next = std::move(temp);
        tail = tail->next.get();
        size_count++;
    }

    std::optional<T> pop_front() {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        if (!head.next) return std::nullopt;

        std::unique_ptr<Node> old_head = std::move(head.next);
        head.next = std::move(old_head->next);

        if (!head.next) {
            tail = &head;
        }

        size_count--;
        return *(old_head->data);
    }

    std::size_t size() const {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return size_count;
    }

    bool empty() const {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return !size();
    }

    void clear() {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        while (head.next) {
            head.next = std::move(head.next->next);
        }
        tail = &head;
        size_count = 0;
    }

    bool remove(const T& value, const Iterator& start_it) {
        std::lock_guard<std::recursive_mutex> lock(mutex);

        if (start_it.getNode() == nullptr) {
            return false;
        }

        Node* current = (start_it.getNode() != nullptr) ? start_it.getNode() : head.next.get();

        Node* prev = &head;
        while (prev->next.get() != nullptr && prev->next.get() != current) {
            prev = prev->next.get();
        }

        while (current) {
            if (current->data && *(current->data) == value) {
                if (prev->next.get() == current) {
                    std::unique_ptr<Node> node_to_remove = std::move(prev->next);
                    prev->next = std::move(node_to_remove->next);
                    size_count--;
                    return true;
                }
            }

            prev = current;
            current = current->next.get();
        }

        return false;
    }

    Iterator begin() {
        return Iterator(head.next.get(), mutex);
    }

    Iterator begin(std::unique_lock<std::recursive_mutex>&& lock) {
        return Iterator(head.next.get(), std::move(lock));
    }

    Iterator end() {
        return Iterator();
    }

    std::unique_lock<std::recursive_mutex> get_lock() {
        return std::unique_lock<std::recursive_mutex>(mutex);
    }
};
