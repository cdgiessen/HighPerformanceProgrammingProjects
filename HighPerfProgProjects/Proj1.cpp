#include <iostream>
#include <string>
int main(int argc, char **argv) {

	std::cout << "Please enter your name below" << std::endl;

	std::string inputName;

	std::cin >> inputName;

	std::cout << "Hello, " << inputName << std::endl;

	system("PAUSE");
}