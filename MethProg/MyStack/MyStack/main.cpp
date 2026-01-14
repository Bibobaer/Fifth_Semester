#include <iostream>
#include "main.hpp"

template <typename U>
void read_stack_top(const MyStack<U>& stack) {
	std::cout << stack.top() << '\n';
}

template <typename T>
void print_stack(MyStack<T>& stack) {
	while (stack.size() != 0) {
		std::cout << stack.top() << '\n'; stack.pop();
	}
	std::cout << '\n';
}

int main(void) {
	//try {
	//	{// Array test
	//		std::cout << "Array Stack" << '\n';
	//		ArrayStack<int> test;
	//		test.push(-1);
	//		test.push(4);
	//		test.push(3);
	//		test.push(2);
	//		test.push(1);

	//		auto test_1(test);
	//		print_stack(test_1);

	//		auto min_el = findMinimumElement(test);
	//		std::cout << "Minimum element: " << min_el << "\n\n";
	//		ArrayStack<int> test_3(std::move(test));
	//		print_stack(test_3);
	//	}
	//	{// List test
	//		std::cout << "List Stack" << '\n';
	//		ListStack<int> stack;
	//		stack.push(1);
	//		stack.push(1);
	//		stack.push(4);
	//		stack.push(2);
	//		stack.push(2);
	//		stack.push(2);

	//		auto new_stack(stack);
	//		print_stack(new_stack);

	//		auto min_cnt = findAllMinimumElements(stack);
	//		auto min_el = min_cnt.first;
	//		std::cout << "Minimum element: " << min_el << ", Count minimum: " << min_cnt.second << "\n\n";

	//		ListStack<int> new_stack_2(std::move(stack));
	//		print_stack(new_stack_2);
	//	}
	//	{// Chunk test
	//		std::cout << "Chunk Stack" << '\n';
	//		ChunkStack<int> stack;
	//		stack.push(1);
	//		stack.push(5);
	//		stack.push(4);
	//		stack.push(3);
	//		stack.push(6);

	//		selected_sort(stack);
	//		print_stack(stack);
	//	}
	//	{// Test Sort
	//		std::cout << "Test Sorts" << '\n';
	//		ArrayStack<int> stack;
	//		stack.push(5);
	//		stack.push(17);
	//		stack.push(1);
	//		stack.push(23);
	//		stack.push(4);
	//		stack.push(6);

	//		ArrayStack<int> temp(stack);
	//		print_stack(temp);

	//		quick_sort(stack);
	//		print_stack(stack);
	//	}
	//}
	//catch (std::exception e) {
	//	std::cout << e.what() << '\n';
	//}

	//runPerformanceTests();
	
	std::string str = "([]{} []{()})";
	_test_brackets(str);

	return 0;
}