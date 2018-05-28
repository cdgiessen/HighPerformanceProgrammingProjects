#include <pthread.h> 

#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <tuple>
#include <limits>

struct Timer {
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
	
	void StartTimer(){
		startTime = std::chrono::high_resolution_clock::now();
	}
	
	void EndTimer(){
		endTime = std::chrono::high_resolution_clock::now();
	}
	
	double GetElapsedNanoseconds(){
		return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
	}
	
	double GetElapsedSeconds() {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() / 1000000000;
	}
};

struct PiThreadData{
	int seed = 0;
	int64_t numsToGenerate;
	int64_t numsInside;
	double timeTaken;	
};

std::tuple<int64_t, double> CalcPiInside(int64_t numsToGenerate, int seed){
	Timer timer;
	timer.StartTimer();
	
	std::mt19937_64 mersenneTwister(seed);
	std::uniform_real_distribution<double> dis(-1.0, 1.0);
	
	int64_t numsInside = 0;
	for(int64_t i = 0; i < numsToGenerate; i++)
	{
		double numX = dis(mersenneTwister);
		double numY = dis(mersenneTwister);
		if (numX*numX + numY*numY <= 1){
			numsInside++;
		}
	}
	std::cout << numsInside << std::endl;
	timer.EndTimer();
	return std::make_tuple (numsInside, timer.GetElapsedNanoseconds());
};

void* RunPiThread (void* data){
	PiThreadData* results = (PiThreadData*)(data);
	std::tie(results->numsInside, results->timeTaken) = CalcPiInside( results->numsToGenerate, results->seed);
	pthread_exit((void*) results);
};


int main(int argc, char *argv[]) {

	int64_t numThreads = 1;
	int64_t numsToGenerate = 1000;
	std::vector<pthread_t> threads;
	std::vector<PiThreadData> results;
	pthread_attr_t attr;
	Timer wallTimer;
	wallTimer.StartTimer();
	
	if(argc == 3){
		numThreads = std::stol(std::string(argv[1]));
		numsToGenerate = std::stol(std::string(argv[2]));
	} else if (argc == 2) {
		numThreads = std::stol(std::string(argv[1]));
	}
	
	/* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	threads.resize(numThreads);
	results.resize(numThreads);
	
	for(int64_t i = 0; i < numThreads; i++){
		results[i].seed = 256 - i;
		results[i].numsToGenerate = numsToGenerate/(numThreads);
		if(i == numThreads - 1)
			results[i].numsToGenerate += numsToGenerate%numThreads;
		
		int rc = pthread_create(&threads[i], &attr, RunPiThread, &(*(results.begin() + i)));
		if (rc) {
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
		}
	}
	
	
	/* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
	for(int64_t i = 0; i < numThreads; i++){
		
		int rc = pthread_join(threads[i], NULL);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
        }
		
	}
	
	int64_t sum_numsInside = 0;
	int64_t sum_timeInside = 0;
	for(int64_t i = 0; i < numThreads; i++){
		sum_numsInside += results[i].numsInside;
		sum_timeInside += results[i].timeTaken;
	}
	wallTimer.EndTimer();
	
	std::cout << "Numbers generated " << numsToGenerate << std::endl;
	std::cout << "Numbers generated per thread " << numsToGenerate/numThreads << std::endl;
	std::cout << "Numbers inside the circle " << sum_numsInside << std::endl;

	double pi_approx =  4.0 * (double)sum_numsInside / (double) (numsToGenerate);
	
	std::cout << "Pi Approximation " << pi_approx << std::endl;
	std::cout << "Wall Clock Time Taken (ns): " << wallTimer.GetElapsedNanoseconds() << std::endl;
	std::cout << "Wall Clock Time Taken (s): " << wallTimer.GetElapsedNanoseconds()*0.000000001 << std::endl;
	std::cout << "Sum of individual thread times (ns): " << sum_timeInside << std::endl;
	std::cout << "Sum of individual thread times (s): " << sum_timeInside*0.000000001 << std::endl;

	return 0;
}