#pragma once
#include <utility>

template <typename T>
class MyStack {
public:

	virtual void push(const T& value) = 0;
	virtual void push(T&& value) = 0;
	virtual void pop() = 0;
	virtual T& top() = 0;
	virtual const T& top() const = 0;

	inline unsigned int size() const {
		return count;
	}
protected:
	unsigned int count = 0;
};