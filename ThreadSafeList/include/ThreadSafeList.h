#pragma once

#include <mutex>
#include <memory>
#include <optional>

template <typename T>
class ThreadSafeList {

	struct Node {
		std::shared_ptr<T> data;
		std::unique_ptr<Node> next;

		Node() : next(nullptr) {}
		explicit Node(T _data) : data(std::make_shared<T>(std::move(_data))), next(nullptr) {}
	};

	Node head;
	mutable std::mutex mutex;
	std::size_t size_list = 0;

public:

	ThreadSafeList() = default;
	~ThreadSafeList() = default;

	ThreadSafeList(const ThreadSafeList&) = delete;
	ThreadSafeList operator=(const ThreadSafeList&) = delete;

	void push_front(T _data) {
		std::unique_ptr<Node> temp = std::make_unique<Node>(std::move(_data));
		std::lock_guard<std::mutex> lock(mutex);
		temp->next = std::move(head.next);
		head.next = std::move(temp);
		size_list++;
	}

	void push_back(T _data) {
		std::unique_ptr<Node> temp = std::make_unique<Node>(std::move(_data));
		std::lock_guard<std::mutex> lock(mutex);

		Node* current = &head;
		while (current->next) {
			current = current->next.get();
		}

		current->next = std::move(temp);
		size_list++;
	}

	std::size_t size() const {
		std::lock_guard<std::mutex> lock(mutex);
		return size_list;
	}

	bool empty() const {
		std::lock_guard<std::mutex> lock(mutex);
		return size_list == 0;
	}

	void clear() {
		std::lock_guard<std::mutex> lock(mutex);
		head.next.reset();
		size_list = 0;
	}

	bool contains(const T& _data) {
		std::lock_guard<std::mutex> lock(mutex);

		Node* current = head.next.get();

		while (current) {
			if (current->data && *(current->data) == _data) return true;
			current = current->next.get();
		}

		return false;
	}

	bool remove(const T& _data) {
		std::lock_guard<std::mutex> lock(mutex);

		Node* current = &head;

		while (current->next) {
			if (current->next->data && *(current->next->data) == _data) {
				std::unique_ptr<Node> node_to_remove = std::move(current->next);
				current->next = std::move(node_to_remove->next);
				size_list--;
				return true;
			}
			current = current->next.get();
		}
		
		return false;
	}

};


