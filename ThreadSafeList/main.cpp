#include <iostream>
#include "include/ThreadSafeList.h"

int main()
{
	ThreadSafeList<int> list;

	list.clear();

	list.push_back(5);

	list.push_front(2);

	std::cout << list.empty() << std::endl;

	std::cout << list.remove(3) << std::endl;

	std::cout << list.remove(5) << std::endl;

	std::cout << list.empty() << std::endl;


	list.clear();

	std::cout << list.empty() << std::endl;

}
