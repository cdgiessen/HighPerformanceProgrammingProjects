#include <mpi.h> 
#include <stdio.h> 
#include <cstdint>

#include <iostream>
#include <string>
#include <chrono>
#include <random>
/*
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
*/

int main(int argc, char *argv[]) {
	int my_rank, comm_sz;
	
	//Timer timer;
	//timer.StartTimer();

	uint64_t numsToGenerate = 16;
	uint64_t localNumsToGenerate = 16;
	uint64_t numsInside = 0;
	
	/* Let the system do what it needs to start up MPI */
	MPI_Init(NULL, NULL);

	/* Get my process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out how many processes are being used */
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	
	if(my_rank == 0) {
		if(argc == 2){
			numsToGenerate = std::stol(std::string(argv[1]));
		}
		
		MPI_Bcast(&numsToGenerate, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
	} else {
		MPI_Bcast(&numsToGenerate, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
	}
	
	localNumsToGenerate = numsToGenerate / comm_sz;
	if(my_rank == comm_sz - 1) //last processor needs the remainder
		localNumsToGenerate += numsToGenerate%comm_sz; 
	
	
	//int seed = 256 - my_rank;
	//std::mt19937_64 mersenneTwister(seed);
	//std::uniform_real_distribution<> dis(-1.0, 1.0);
	
	//timer.StartTimer();
	
	for(uint64_t i = 0; i < localNumsToGenerate; i++){
		double numX = std::rand() * 2 - 1;//dis(mersenneTwister);
		double numY = std::rand() * 2 - 1;//dis(mersenneTwister);
		if (numX*numX + numY*numY <= 1){
			numsInside++;
		}
	}
	//timer.EndTimer();
	
	uint64_t globalSumOfNums;
	
	if(my_rank == 0){
		MPI_Reduce(&numsInside, &globalSumOfNums, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	}else {
		MPI_Reduce(&numsInside, &globalSumOfNums, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	}
	
	if(my_rank == 0){
		std::cout << "Numbers generated " << numsToGenerate << std::endl;
		std::cout << "Numbers inside the circle " << numsInside << std::endl;
	
		double pi_approx =  4.0 * (double)globalSumOfNums / (double) (numsToGenerate);
		
		std::cout << "Pi Approximation " << pi_approx << std::endl;
		//	std::cout << "Time taken: " << timer.GetElapsedTimeNanosecondsChronoTime() << std::endl;
	}
	
	/* Shut down MPI */
	MPI_Finalize();
	
	return 0;
}