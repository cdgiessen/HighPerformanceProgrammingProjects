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

int numThreads = 1;
std::vector<pthread_t> threads;
pthread_attr_t attr;

void* task(void* t){
	
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	Timer wallTimer, createTimer, joinTimer;
	wallTimer.StartTimer();

	if (argc == 2) {
		numThreads = std::stoi(std::string(argv[1]));
	}
	
	/* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	threads.resize(numThreads);
	
	long t;
	createTimer.StartTimer();
	for(int i = 0; i < numThreads; i++){
		int rc = pthread_create(&threads[i], &attr, task, (void*)t);
		if (rc) {
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
		}
	}
	createTimer.EndTimer();
	/* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
	joinTimer.StartTimer();
	for(int i = 0; i < numThreads; i++){
		
		int rc = pthread_join(threads[i], NULL);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
        }
		
	}
	joinTimer.EndTimer();
	
	wallTimer.EndTimer();
	
	std::cout << "Timing creation and joining of " << numThreads << " threads: " << "\n" << "\n";
	std::cout << "Creating threads (ns)" << createTimer.GetElapsedNanoseconds() << "\n";
	std::cout << "Creating threads (s) " << createTimer.GetElapsedNanoseconds()*0.000000001 << "\n";
	std::cout << "Average create time (s) " << createTimer.GetElapsedNanoseconds()*0.000000001/numThreads << "\n" << "\n";
	std::cout << "Joining threads (ns)" << joinTimer.GetElapsedNanoseconds() << "\n";
	std::cout << "Joining threads (s) " << joinTimer.GetElapsedNanoseconds()*0.000000001 << "\n";
	std::cout << "Average join time (s) " << joinTimer.GetElapsedNanoseconds()*0.000000001/numThreads << "\n";
	std::cout << "\n" << "Wall Clock Time Taken (ns): " << wallTimer.GetElapsedNanoseconds() << "\n";
	std::cout << "Wall Clock Time Taken (s): " << wallTimer.GetElapsedNanoseconds()*0.000000001 << std::endl;
	
	return 0;
}