#include <pthread.h> 
#include <semaphore.h>

#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <tuple>
#include <limits>

/*
Write a Pthreads program that implements the trapezoidal rule. Use a shared
variable for the sum of all the threads’ computations. Use busy-waiting,
mutexes, and semaphores to enforce mutual exclusion in the critical section.
What advantages and disadvantages do you see with each approach?
*/

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


struct ThreadData;

double a = 0;
double b = 1;
double divisionWidth;
int64_t numDivisions = 16;

int numThreads = 1;
std::vector<pthread_t> threads;
std::vector<ThreadData> threadData;
double result;
pthread_attr_t attr;

pthread_spinlock_t spinMutexSum;
pthread_mutex_t mutexsum;

sem_t count_sem;
sem_t barrier_sem;
int counter = 0;

struct ThreadData{
	int threadID = 0;
	int divisionsToRun = 1;
	//busy waiting, mutexes, and semaphores
	
	double amountUnder = 0;	
	double timeTaken = 0;	
};

double f(double x){
	return x*x;
}

double Trap(double  local_a, double  local_b, int64_t local_n) {
    double integral = (f(local_a) + f(local_b))/2.0;
    for (int64_t i = 1; i <= local_n-1; i++) {
        integral += f(local_a + i*divisionWidth);
    }
    integral = integral*divisionWidth;
    return integral;
}

void* RunThreadBusyWait (void* dataIn){
	ThreadData* data = (ThreadData*)(dataIn);
	Timer timer;
	timer.StartTimer();
	
	double local_a = a + data->threadID*data->divisionsToRun*divisionWidth;
    double local_b = local_a + data->divisionsToRun*divisionWidth;

    double value = Trap(local_a, local_b, data->divisionsToRun);
	
	pthread_spin_lock(&spinMutexSum);
		result += value;
    pthread_spin_unlock(&spinMutexSum);
	
	timer.EndTimer();
	data->amountUnder = value;
	data->timeTaken = timer.GetElapsedNanoseconds();
	
	pthread_exit((void*) data);
};

void* RunThreadMutex (void* dataIn){
	ThreadData* data = (ThreadData*)(dataIn);
	Timer timer;
	timer.StartTimer();
	
	double local_a = a + data->threadID*data->divisionsToRun*divisionWidth;
    double local_b = local_a + data->divisionsToRun*divisionWidth;

    double value = Trap(local_a, local_b, data->divisionsToRun);
	
	pthread_mutex_lock (&mutexsum);
    result += value;
    pthread_mutex_unlock (&mutexsum);
	
	timer.EndTimer();
	data->amountUnder = value;
	data->timeTaken = timer.GetElapsedNanoseconds();
	
	pthread_exit((void*) data);
};

void* RunThreadSemaphore (void* dataIn){
	ThreadData* data = (ThreadData*)(dataIn);
	Timer timer;
	timer.StartTimer();
	
	double local_a = a + data->threadID*data->divisionsToRun*divisionWidth;
    double local_b = local_a + data->divisionsToRun*divisionWidth;

    double value = Trap(local_a, local_b, data->divisionsToRun);
	sem_wait(&count_sem);
	result += value;
	if(counter == numThreads - 1){
		counter = 0;
		sem_post(&count_sem);
		for(int j = 0; j < numThreads - 1; j++){
			sem_post(&barrier_sem);
		}
	} else {
		counter++;
		sem_post(&count_sem);
		sem_post(&barrier_sem);
	}
	
	timer.EndTimer();
	data->amountUnder = value;
	data->timeTaken = timer.GetElapsedNanoseconds();
	pthread_exit((void*) data);
};

void RunTasks( void* (*task)(void*), bool isSemaphoreTask){
	Timer taskTimer;
	taskTimer.StartTimer();
	
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for(int i = 0; i < numThreads; i++){
		threadData[i].threadID = i; 
		threadData[i].divisionsToRun = numDivisions/numThreads;
		if(i == numThreads - 1)
			threadData[i].divisionsToRun += numDivisions%numThreads;
		
		int rc = pthread_create(&threads[i], &attr, task, &(*(threadData.begin() + i)));
		if (rc) {
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
		}
	}

	/* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
	for(int i = 0; i < numThreads; i++){
		
		int rc = pthread_join(threads[i], NULL);
		if (rc) {
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
        }
	}
	
	double sum_amountUnder = 0;
	int64_t sum_timeInside = 0;
	for(int i = 0; i < numThreads; i++){
		sum_amountUnder += threadData[i].amountUnder;
		sum_timeInside += threadData[i].timeTaken;
	}
	taskTimer.EndTimer();
	
	std::cout << "Amount under function: " << (double)sum_amountUnder << "\n";
	std::cout << "Added result of trap rule: " << (double)result << "\n";
	
	std::cout << "Run Time of Task (ns): " << taskTimer.GetElapsedNanoseconds() << "\n";
	std::cout << "Run Time of Task (s): " << taskTimer.GetElapsedNanoseconds()*0.000000001 << "\n";
	std::cout << "Sum of individual thread times (ns): " << sum_timeInside << "\n";
	std::cout << "Sum of individual thread times (s): " << sum_timeInside*0.000000001 << std::endl;
	
	result = 0;
}

int main(int argc, char *argv[]) {
	Timer wallTimer;
	wallTimer.StartTimer();

	if(argc == 3){
		numThreads = std::stoi(std::string(argv[1]));
		numDivisions = std::stol(std::string(argv[2]));
	} else if (argc == 2) {
		numThreads = std::stoi(std::string(argv[1]));
	}
	
	/* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	threads.resize(numThreads);
	threadData.resize(numThreads);
	
	pthread_spin_init(&spinMutexSum, PTHREAD_PROCESS_PRIVATE);
	pthread_mutex_init(&mutexsum, NULL);
	sem_init(&count_sem, 0, 1);
	sem_init(&barrier_sem, 0, 0);	
	
	divisionWidth = (b - a)/numDivisions;
	
	std::cout << "Divisions: " << numDivisions << "\n";
	std::cout << "Numbers of divisions per thread " << numDivisions/numThreads << std::endl;
		
	std::cout << "\n" << "Running trap with busy waiting" << std::endl;
	RunTasks(RunThreadBusyWait, false);
	std::cout << "\n" << "Running trap with mutexes" << std::endl;
	RunTasks(RunThreadMutex, false);
	std::cout << "\n" << "Running trap with semaphores" << std::endl;
	RunTasks(RunThreadSemaphore, true);
	
	wallTimer.EndTimer();
	std::cout << "\n" << "Wall Clock Time Taken (ns): " << wallTimer.GetElapsedNanoseconds() << "\n";
	std::cout << "Wall Clock Time Taken (s): " << wallTimer.GetElapsedNanoseconds()*0.000000001 << std::endl;
	
	pthread_mutex_destroy(&mutexsum);
	return 0;
}