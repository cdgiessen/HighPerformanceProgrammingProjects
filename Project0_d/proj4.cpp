#include <chrono>

#include <iostream>

#include <cmath>


int main(int argc, char *argv[]) 
{
	long numRepetitions = 100000;
	if (argc >= 2) {
		numRepetitions = *argv[1];
	}


	std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
	std::chrono::nanoseconds timeTaken;
	double dummyVal = 0;
	clock_t startClockCycles, endClockCycles;
	double timeSpent;

	/*
	MULTIPLICATION
	*/

	startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal *= dummyVal;
	}
	endTime = std::chrono::high_resolution_clock::now();
	timeTaken = (endTime - startTime);

	std::cout << "Time taken to compute " << numRepetitions << " multiplication computations: " << timeTaken.count() << " nanoseconds" << std::endl;

	startClockCycles = clock();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal *= dummyVal;
	}
	endClockCycles = clock();
	timeSpent = (double)(endClockCycles - startClockCycles) / CLOCKS_PER_SEC;

	std::cout << "Time taken to computer " << numRepetitions << " multiplication computations using c style " << timeSpent << std::endl;

	/*
	DIVISION
	*/

	startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal /= dummyVal;
	}
	endTime = std::chrono::high_resolution_clock::now();
	timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

	std::cout << "Time taken to compute " << numRepetitions << " division computations: " << timeTaken.count() << " nanoseconds" << std::endl;

	startClockCycles = clock();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal /= dummyVal;
	}
	endClockCycles = clock();
	timeSpent = (double)(endClockCycles - startClockCycles) / CLOCKS_PER_SEC;

	std::cout << "Time taken to computer " << numRepetitions << " division computations using c style " << timeSpent << std::endl;

	/*
	SQRT
	*/

	startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal = std::sqrt(dummyVal);
	}
	endTime = std::chrono::high_resolution_clock::now();
	timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

	std::cout << "Time taken to compute " << numRepetitions << " sqrt computations: " << timeTaken.count() << " nanoseconds" << std::endl;

	startClockCycles = clock();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal = std::sqrt(dummyVal);
	}
	endClockCycles = clock();
	timeSpent = (double)(endClockCycles - startClockCycles) / CLOCKS_PER_SEC;

	std::cout << "Time taken to computer " << numRepetitions << " sqrt computations using c style " << timeSpent << std::endl;

	/*
	SIN
	*/

	startTime = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal = std::sin(dummyVal);
	}
	endTime = std::chrono::high_resolution_clock::now();
	timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);

	std::cout << "Time taken to compute " << numRepetitions << " sin computations: " << timeTaken.count() << " nanoseconds" << std::endl;

	startClockCycles = clock();
	for (int i = 0; i < numRepetitions; i++)
	{
		dummyVal = std::sin(dummyVal);
	}
	endClockCycles = clock();
	timeSpent = (double)(endClockCycles - startClockCycles) / CLOCKS_PER_SEC;

	std::cout << "Time taken to computer " << numRepetitions << " sin computations using c style " << timeSpent << std::endl;



	system("PAUSE");
}

