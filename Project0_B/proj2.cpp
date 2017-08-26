#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>

double calcPiUsingArchimedes(int n) {
	
	double sideLength = std::sqrt(2.0); //length of initial side
	double perimeter = sideLength * 4;
	double numSides = 4.0;

	for (int i = 0; i < n; i++)
	{
		sideLength = std::sqrt(std::pow(1 - std::sqrt(1 - std::pow(sideLength / 2.0, 2)), 2.0) + std::pow(sideLength / 2.0, 2.0));
		perimeter = 2.0 * numSides * sideLength;
		numSides = numSides * 2L;
	}
	return perimeter/2.0;
}

int main(int argc, char **argv) {

	std::cout << "Archemedies Algorithm" << std::endl;

	for (int i = 0; i < 100; i++)
	{
		double pi = calcPiUsingArchimedes(i);
		std::cout << "Caclulated to " << i << " iterations gives " << std::setprecision(std::numeric_limits<long double>::digits10 + 1) << pi << std::endl;
	}


	system("PAUSE");
}
