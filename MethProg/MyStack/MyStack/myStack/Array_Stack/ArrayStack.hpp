#pragma once
#include "../MyStack.hpp"
#include <iostream>

constexpr unsigned int initial_capacity = 1;

enum alloc_type {
	alloc_more,
	alloc_less,
	no_alloc
};

template <typename T>
class ArrayStack : public MyStack<T> {
public:
	ArrayStack() : data_array(nullptr), capacity(0) {}

	ArrayStack(const ArrayStack<T>& other) : data_array(nullptr), capacity(other.capacity){
		this->count = other.count;

		if (capacity != 0) {
			this->data_array = new T[capacity];
			std::copy_n(other.data_array, this->count, this->data_array);
		}
	}
	ArrayStack(ArrayStack<T>&& other) : data_array(other.data_array), capacity(other.capacity) {
		this->count = other.count;

		other.data_array = nullptr;
		other.capacity = 0;
		other.count = 0;
	}
	~ArrayStack() {
		delete[] this->data_array;
	}

	ArrayStack<T>& operator=(const ArrayStack<T>& other) {
		if (this != &other) {
			this->count = other.count;
			capacity = other.capacity;

			if (capacity != 0) {
				this->~ArrayStack();
				this->data_array = new T[capacity];
				std::copy_n(other.data_array, this->count, this->data_array);
			}
		}
		return *this;
	}

	ArrayStack<T>& operator=(ArrayStack<T>&& other) {
		if (this != &other) {
			this->~ArrayStack();
			data_array = other.data_array;
			capacity = other.capacity;
			this->count = other.count;

			other.data_array = nullptr;
			other.capacity = 0;
			other.count = 0;
		}

		return *this;
	}

	void push(const T& value) override {
		if (data_array == nullptr)
			reallocate(initial_capacity);
		if (check_capacity() == alloc_more)
			reallocate(this->capacity * 2);

		this->data_array[this->count++] = value;
		return;
	}
	void push(T&& value) override {
		if (data_array == nullptr)
			reallocate(initial_capacity);
		if (check_capacity() == alloc_more)
			reallocate(this->capacity * 2);

		this->data_array[this->count++] = std::move(value);
		return;
	}
	void pop() override {
		if (this->count == 0 || data_array == nullptr) {
			throw std::exception("You cant pop element from empty stack");
		}
		if (check_capacity() == alloc_less)
			reallocate(this->capacity / 2);
		this->count--;
		this->data_array[this->count] = T();
	}

	T& top() override {
		if (this->count == 0 || data_array == nullptr) {
			throw std::exception("Stack is empty");
		}
		return this->data_array[this->count - 1];
	}

	const T& top() const override {
		if (this->count == 0 || data_array == nullptr) {
			throw std::exception("Stack is empty");
		}
		return this->data_array[this->count - 1];
	}

private:
	alloc_type check_capacity() {
		if (this->count == 0)
			return no_alloc;
		else if (this->count == this->capacity)
			return alloc_more;
		else if (this->capacity / this->count >= 4)
			return alloc_less;
		return no_alloc;
	}

	void reallocate(unsigned int new_size) {
		T* new_data = new T[new_size];

		for (size_t i = 0; i < this->count; i++)
			new_data[i] = std::move(this->data_array[i]);
		delete[] this->data_array;

		this->data_array = new_data;
		this->capacity = new_size;
	}

	T* data_array;
	unsigned int capacity;
};