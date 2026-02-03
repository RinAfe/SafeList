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

    public:
        Iterator() : current(nullptr) {}
        explicit Iterator(Node* node) : current(node) {}

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
        return size_count == 0;
    }

    void clear() {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        while (head.next) {
            head.next = std::move(head.next->next);
        }
        tail = &head;
        size_count = 0;
    }

    bool erase(const T& value) {
        std::lock_guard<std::recursive_mutex> lock(mutex);

        Node* prev = &head;
        while (prev->next) {
            if (prev->next->data && *(prev->next->data) == value) {
                std::unique_ptr<Node> temp = std::move(prev->next);
                prev->next = std::move(temp->next);

                if (temp.get() == tail) {
                    tail = prev;
                }

                size_count--;
                return true;
            }
            prev = prev->next.get();
        }
        return false;
    }

    Iterator erase(Iterator it) {
        std::lock_guard<std::recursive_mutex> lock(mutex);

        if (it.getNode() == nullptr) {
            return end();
        }

        Node* node_to_remove = it.getNode();
        Node* prev = &head;

        while (prev->next && prev->next.get() != node_to_remove) {
            prev = prev->next.get();
        }

        if (prev->next && prev->next.get() == node_to_remove) {
            Node* next_node = node_to_remove->next.get();
            std::unique_ptr<Node> temp = std::move(prev->next);
            prev->next = std::move(temp->next);

            if (temp.get() == tail) {
                tail = prev;
            }

            size_count--;
            return Iterator(next_node);
        }

        return end();
    }

    bool remove(const T& value, const Iterator& first, const Iterator& last) {
        std::lock_guard<std::recursive_mutex> lock(mutex);

        bool removed = false;
        Node* current = first.getNode();
        Node* prev = &head;

        while (prev->next && prev->next.get() != current) {
            prev = prev->next.get();
        }

        while (current && current != last.getNode()) {
            if (current->data && *(current->data) == value) {
                std::unique_ptr<Node> temp = std::move(prev->next);
                prev->next = std::move(temp->next);
                current = prev->next.get();

                if (temp.get() == tail) {
                    tail = prev;
                }

                size_count--;
                removed = true;
            } else {
                prev = current;
                current = current->next.get();
            }
        }

        return removed;
    }

    bool remove(const T& value) {
        return remove(value, begin(), end());
    }

    Iterator begin() {
        std::lock_guard<std::recursive_mutex> lock(mutex);
        return Iterator(head.next.get());
    }

    Iterator end() {
        return Iterator();
    }

    std::unique_lock<std::recursive_mutex> get_lock() {
        return std::unique_lock<std::recursive_mutex>(mutex);
    }
};