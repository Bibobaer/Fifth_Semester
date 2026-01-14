#pragma once
#include "../MyStack.hpp"
#include <iostream>

template <typename T>
class ListStack : public MyStack<T>{
public:
	ListStack() = default;

	ListStack(const ListStack<T>& other) {
		this->count = other.count;

		this->Top = new Node(other.Top->value);
		Node* cur_this = this->Top;
		Node* cur_other = other.Top->next;

		while (cur_other != nullptr) {
			cur_this->next = new Node(cur_other->value);
			cur_this = cur_this->next;
			cur_other = cur_other->next;
		}
	}

	ListStack(ListStack<T>&& other) : Top(other.Top) {
		this->count = other.count;

		other.Top = nullptr;
		other.count = 0;
	}

	~ListStack() {
		while (Top != nullptr)
			this->pop();
	}

	ListStack<T>& operator=(const ListStack<T>& other) {
		if (this != &other) {
			this->~ListStack();
			this->count = other.count;

			this->Top = new Node(other.Top->value);
			Node* cur_this = this->Top;
			Node* cur_other = other.Top->next;

			while (cur_other != nullptr) {
				cur_this->next = new Node(cur_other->value);
				cur_this = cur_this->next;
				cur_other = cur_other->next;
			}
		}
		return *this;
	}

	ListStack<T>& operator=(ListStack<T>&& other) {
		if (this != &other) {
			this->~ListStack();
			Top = other.Top;
			this->count = other.count;

			other.Top = nullptr;
			other.count = 0;
		}
		return *this;
	}

	void push(const T& value) override {
		Node* new_node = new Node(value);
		new_node->next = Top;
		Top = new_node;
		this->count++;
	}

	void push(T&& value) override {
		Node* new_node = new Node(std::move(value));
		new_node->next = Top;
		Top = new_node;
		this->count++;
	}

	void pop() override {
		if (Top == nullptr)
			return;
		Node* deleted_node = Top;
		Top = Top->next;
		delete deleted_node;
		this->count--;
	}

	T& top() override {
		if (Top == nullptr)
			throw std::exception("Stack is empty");
		return Top->value;
	}

	const T& top() const override {
		if (Top == nullptr)
			throw std::exception("Stck is empty");
		return Top->value;
	}

private:
	struct Node {
		T value;
		Node* next;
		Node(const T& data) : value(data), next(nullptr) {}
		Node(T&& data) : value(std::move(data)), next(nullptr) {}
	}* Top = nullptr;
};