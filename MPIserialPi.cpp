#include <mpi.h> 

#include <iostream>
#include <string>
#include <chrono>
#include <random>

struct Timer {
	double mpiTime_start = 0.0, mpiTime_end = 0.0;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
	
	void StartTimer(){
		mpiTime_start = MPI_Wtime(); 
		startTime = std::chrono::high_resolution_clock::now();
	}
	
	void EndTimer(){
		mpiTime_end = MPI_Wtime();
		endTime = std::chrono::high_resolution_clock::now();
	}
	
	double GetElapsedTimeNanosecondsChronoTime(){
		return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
	}
	
	double GetElapsedTimeMPITime(){
		return mpiTime_end - mpiTime_start;
	}
};

int main(int argc, char *argv[]) {
	Timer timer;
	//timer.StartTimer();

	int numsToGenerate = 16;
	int numsInside = 0;
	
	int seed = 0;
	std::mt19937_64 mersenneTwister(seed);
	std::uniform_real_distribution<> dis(-1.0, 1.0);

	if(argc == 2){
		numsToGenerate = std::stoi(std::string(argv[1]));
	}
	
	timer.StartTimer();
	
	for(int i = 0; i < numsToGenerate; i++){
		double numX = dis(mersenneTwister);
		double numY = dis(mersenneTwister);
		if (numX*numX + numY*numY <= 1){
			numsInside++;
		}
	}
	timer.EndTimer();
	
	std::cout << "Numbers generated " << numsToGenerate << std::endl;
	std::cout << "Numbers inside the circle " << numsInside << std::endl;

	double pi_approx =  4.0 * (double)numsInside / (double) (numsToGenerate);
	
	std::cout << "Pi Approximation " << pi_approx << std::endl;
	std::cout << "Time taken: " << timer.GetElapsedTimeNanosecondsChronoTime() << std::endl;
	return 0;
}