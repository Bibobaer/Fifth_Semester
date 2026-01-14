#pragma once
#include "./MyStack.hpp"
#include <random>

template <typename stack_t>
auto findMinimumElement(stack_t& stack) {
	if (stack.size() == 0) {
		throw std::exception("You cant find minimum element in stack, because stack is empty");
	}

	stack_t temp;
	auto minimum = stack.top();

	while (stack.size() != 0) {
		auto current_elem = stack.top(); stack.pop();
		if (current_elem < minimum) {
			minimum = current_elem;
		}
		temp.push(current_elem);
	}

	bool first_min = false;
	while (temp.size() != 0) {
		if (temp.top() == minimum && !first_min)
			first_min = true;
		else if (temp.top() != minimum) {
			stack.push(temp.top());
		}
		temp.pop();
	}

	return minimum;
}

template <typename stack_t>
auto findAllMinimumElements(stack_t& stack) {
	if (stack.size() == 0) {
		throw std::exception("You cant find minimum element in stack, because stack is empty");
	}

	stack_t temp;
	auto minimum = stack.top();
	size_t minCount = 0;

	while (stack.size() != 0) {
		auto current_elem = stack.top(); stack.pop();
		if (current_elem < minimum) {
			minimum = current_elem;
			minCount = 1;
		}
		else if (current_elem == minimum) {
			minCount++;
		}
		temp.push(current_elem);
	}

	while (temp.size() != 0) {
		if (temp.top() == minimum) {
			temp.pop();
			continue;
		}
		stack.push(temp.top()); temp.pop();
	}
	return std::make_pair( minimum, minCount );
}

template <typename stack_t>
auto getRandomElement(stack_t& stack) {
	if (stack.size() == 0) {
		throw std::out_of_range("Stack is empty");
	}
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<size_t> dist(0, stack.size() - 1);
	size_t randomIndex = dist(gen);

	stack_t temp;
	auto res = stack.top();
	size_t currIndex = 0;

	while (stack.size() != 0) {
		auto current = stack.top();
		stack.pop();
		temp.push(current);

		if (currIndex == randomIndex) {
			res = current;
		}
		currIndex++;
	}

	// Восстанавливаем исходный стек
	while (temp.size() != 0) {
		stack.push(temp.top());
		temp.pop();
	}

	return res;
}

template <typename stack_t>
void selected_sort(stack_t& stack) {
	if (stack.size() <= 1) return;
	stack_t temp;

	while (stack.size() != 0) {
		auto min_elements = findAllMinimumElements(stack);
		for (int i = 0; i < min_elements.second; i++) {
			temp.push(min_elements.first);
		}
	}
	while (temp.size() != 0) {
		stack.push(temp.top());
		temp.pop();
	}
}

template <typename stack_t>
void quick_sort(stack_t& stack) {
	if (stack.size() <= 1) return;
	stack_t small, equal, large;

	auto pivot = getRandomElement(stack);

	while (stack.size() != 0) {
		auto current = stack.top(); stack.pop();
		if (current < pivot)
			small.push(current);
		else if (current > pivot)
			large.push(current);
		else
			equal.push(current);
	}

	quick_sort(small);
	quick_sort(large);

	stack_t temp;

	while (small.size() != 0) {
		temp.push(small.top());
		small.pop();
	}
	
	while (equal.size() != 0) {
		temp.push(equal.top());
		equal.pop();
	}

	while (large.size() != 0) {
		temp.push(large.top());
		large.pop();
	}

	while (temp.size() != 0) {
		stack.push(temp.top());
		temp.pop();
	}
}

template <typename stack_t>
void transferWhileIncreasing(stack_t& src, stack_t& dest) {
	if (src.size() == 0) return;

	dest.push(std::move(src.top()));
	src.pop();

	while (src.size() != 0 && src.top() >= dest.top()) {
		dest.push(std::move(src.top()));
		src.pop();
	}
}

template <typename stack_t>
void mergeSortedStacks(stack_t& s1, stack_t& s2, stack_t& t) {
	while (s1.size() != 0 && s2.size() != 0) {
		if (s1.top() > s2.top()) {
			t.push(std::move(s1.top()));
			s1.pop();
		}
		else if (s1.top() < s2.top()) {
			t.push(std::move(s2.top()));
			s2.pop();
		}
		else {
			t.push(std::move(s1.top()));
			t.push(std::move(s2.top()));
			s1.pop(); s2.pop();
		}
	}

	while (s1.size() != 0) {
		t.push(std::move(s1.top()));
		s1.pop();
	}
	while (s2.size() != 0) {
		t.push(std::move(s2.top()));
		s2.pop();
	}
}

template <typename stack_t>
void swap(stack_t& a, stack_t& b) {
	stack_t temp = std::move(a);
	a = std::move(b);
	b = std::move(temp);
}

//Реализовать следующую схему сортировки стеков с помощью слияний :
//1. Создаем три вспомогательных стека s1, s2, t
//2. Пока не пуст исходный стек s
//- извлекаем возрастающую последовательность в s1 и s2
//- сливаем s1 и s2 в t
//- свопаем s и t(для этого перегрузить функцию std::swap для самописных стеков)
//3. Повторяем шаг 2 пока внутри него происходит более одной процедуры слияния

template <typename stack_t>
void mergeSort(stack_t& stack) {
	if (stack.size() <= 1) return;
	
	bool need_another_pass;
	stack_t s1, s2, t;

	do {
		need_another_pass = false;
		int merge_count = 0;

		while (stack.size() != 0) {
			transferWhileIncreasing(stack, s1);
			if (stack.size() == 0) {
				while (s1.size() > 0) {
					t.push(std::move(s1.top()));
					s1.pop();
				}
			}
			else {
				transferWhileIncreasing(stack, s2);
				merge_count++;
				need_another_pass = true;

				mergeSortedStacks(s1, s2, t);
			}
		}

		if (merge_count < 1) {
			need_another_pass = false;
		}
		std::swap(stack, t);

	} while (need_another_pass);
}