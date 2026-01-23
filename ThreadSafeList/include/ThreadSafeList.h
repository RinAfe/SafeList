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
    mutable std::recursive_mutex mutex;
    std::size_t size_count = 0;

public:
    class Iterator {
    private:
        Node* current;
        std::unique_lock<std::recursive_mutex> lock;

    public:
        Iterator(Node* node, std::recursive_mutex& mtx, bool already_locked = false)
            : current(node) {
            if (!already_locked) {
                lock = std::unique_lock<std::recursive_mutex>(mtx);
            }
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
    };

    ThreadSafeList() = default;
    ~ThreadSafeList() = default;

    ThreadSafeList(const ThreadSafeList&) = delete;
    ThreadSafeList& operator=(const ThreadSafeList&) = delete;

    void push_front(T _data) {
        std::unique_ptr<Node> temp = std::make_unique<Node>(std::move(_data));
        std::lock_guard<std::recursive_mutex> lock(mutex);
        temp->next = std::move(head.next);
        head.next = std::move(temp);
        size_count++;
    }

    void push_back(T _data) {
        std::unique_ptr<Node> temp = std::make_unique<Node>(std::move(_data));
        std::lock_guard<std::recursive_mutex> lock(mutex);
        Node* current = &head;
        while (current->next) {
            current = current->next.get();
        }
        current->next = std::move(temp);
        size_count++;
    }

    std::optional<T> pop_front() {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        if (!head.next) return std::nullopt;
        std::unique_ptr<Node> old_head = std::move(head.next);
        head.next = std::move(old_head->next);
        size_count--;
        return *(old_head->data);
    }

    std::size_t size() const {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return size_count;
    }

    bool empty() const {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return size_count == 0;
    }

    void clear() {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        while (head.next) {
            head.next = std::move(head.next->next);
        }
        size_count = 0;
    }

    bool contains(const T& _data) const {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        Node* current = head.next.get();
        while (current) {
            if (current->data && *(current->data) == _data) return true;
            current = current->next.get();
        }
        return false;
    }

    bool remove(const T& _data) {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        Node* current = &head;
        while (current->next) {
            if (current->next->data && *(current->next->data) == _data) {
                std::unique_ptr<Node> node_to_remove = std::move(current->next);
                current->next = std::move(node_to_remove->next);
                size_count--;
                return true;
            }
            current = current->next.get();
        }
        return false;
    }

    Iterator begin() {
        return Iterator(head.next.get(), mutex);
    }

    Iterator end() {
        return Iterator(nullptr, mutex, true);
    }
};