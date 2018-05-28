#include <mpi.h> 
#include <stdio.h> 
#include <cstdint>

#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <vector>
#include <algorithm>
#include <functional>

int numberRange = 100000000;

void GenerateNums(std::vector<int> &vecToFill, int my_rank){
	int seed = 256 - my_rank;
	std::mt19937 mersenneTwister(seed);
	std::uniform_int_distribution<> dis(-numberRange, numberRange);
	
	std::generate(vecToFill.begin(), vecToFill.end(), std::bind(dis, std::ref(mersenneTwister)));
};

void Merge(std::vector<int> &vec, std::vector<int> &toInsert){
	vec.reserve(toInsert.size());
	vec.insert(vec.end(), toInsert.begin(), toInsert.end());
		
	std::inplace_merge(vec.begin(), vec.begin() + toInsert.size(), vec.end());
	
}

int main(int argc, char *argv[]) {
	int my_rank, comm_sz;

	uint64_t totalNumsToSort = 16;
	uint64_t localNumsToSort = 16;
	uint64_t extraNumsToSort = 0;
	std::vector<int> nums;
	std::vector<int> outputNums;
	
	bool printOutputnums = true;
	
	double wallS, wallE, genS, genE, sortS, sortE, mergeS, mergeE;
	wallS = MPI_Wtime();
	
	/* Let the system do what it needs to start up MPI */
	MPI_Init(NULL, NULL);

	/* Get my process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out how many processes are being used */
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	
	//MPI_Op myOp; 
	//MPI_Datatype listType;
	//int MPI_Op_create(merge, true, *myOp);
	
	// Get input
	if(my_rank == 0) {
		if(argc == 2){
			totalNumsToSort = std::stol(std::string(argv[1]));
		}
		if(argc == 3){
			totalNumsToSort = std::stol(std::string(argv[1]));
			printOutputnums = std::stoi(std::string(argv[2]));
		}
		
		MPI_Bcast(&totalNumsToSort, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	} else {
		MPI_Bcast(&totalNumsToSort, 1, MPI_INT, 0, MPI_COMM_WORLD);

	}
	
	localNumsToSort = totalNumsToSort / comm_sz;
	extraNumsToSort = totalNumsToSort%comm_sz; 

	if(my_rank == 0) {
		nums.resize(localNumsToSort + extraNumsToSort);
	} else {
		nums.resize(localNumsToSort);
	}
	genS = MPI_Wtime(); 
	GenerateNums(nums, my_rank);
	genE = MPI_Wtime(); 

	//initial step of mergesorting generated nums
	sortS = MPI_Wtime();
	std::sort(nums.begin(), nums.end());
	sortE = MPI_Wtime();
	outputNums.insert(outputNums.end(), nums.begin(), nums.end());
	
	int cur_threadNum = my_rank;
	int num_iterations = 0;
	
	mergeS = MPI_Wtime();
	while((my_rank != 0 && cur_threadNum % 2 == 0) 
		|| (my_rank == 0 && num_iterations < std::log(comm_sz)/std::log(2))){
		
		//prepare to get nums (make sure it has enough space)
		std::vector<int> incomingVec(localNumsToSort * std::pow(2, num_iterations));
		
		//receive nums;
		int err;
		err = MPI_Recv(incomingVec.data(), incomingVec.size(), MPI_INT, my_rank + std::pow(2, num_iterations), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (err != MPI_SUCCESS){
			
		}
		
		Merge(outputNums, incomingVec);
		
		cur_threadNum /= 2;
		num_iterations++;
		
		//std::cout << "my rank " << my_rank << " cur_threadNum " << cur_threadNum << " num_iterations << " << num_iterations << std::endl;

	}
	if(my_rank != 0){
		//send
		MPI_Send(outputNums.data(), outputNums.size(), MPI_INT, my_rank - std::pow(2, num_iterations), 0, MPI_COMM_WORLD);
	}
	mergeE = MPI_Wtime();
	MPI_Barrier(MPI_COMM_WORLD);
	
	wallE = MPI_Wtime();
	
	//print output
	if(my_rank == 0){
		std::cout << "Wall clock time " << wallE - wallS << std::endl;
		std::cout << "Number Generation time for " << localNumsToSort << " numbers: " << genE - genS << std::endl;
		std::cout << "Initial Sorting time: " << sortE - sortS << std::endl;
		std::cout << "Merge time: " << mergeE - mergeS << std::endl;
		
		std::cout << "Numbers sorted " << outputNums.size() << std::endl;
		if(printOutputnums){ //only print first 20 + last 20 unless asked
			std::cout << "Sorted Numbers:" << std::endl;
			for(auto it = outputNums.begin(); it != outputNums.end(); it++)
				std::cout << *it << ' ';
			std::cout << std::endl << " ";
			
		} else {
			std::cout << "First 20 sorted numbers: " << std::endl;
			for(auto it = outputNums.begin(); it != outputNums.begin() + 20; it++)
				std::cout << *it << ' ';
			std::cout << std::endl << "Last 20 sorted numbers: ";
			for(auto it = outputNums.end() - 20; it != outputNums.end(); it++)
				std::cout << *it << ' ';
			std::cout << std::endl;
		}
	}
	
	/* Shut down MPI */
	MPI_Finalize();
	
	return 0;
}