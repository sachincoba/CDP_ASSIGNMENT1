#include <iostream> 
#include <fstream>
#include <pthread.h> 
#include <queue> 
#include <stdlib.h> 
#include <sys/types.h>
#include <limits.h>
#include <time.h>
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
using namespace std;


static unsigned int TotalThread;
static int TotalProducer;

vector<int> election_queue;
pthread_mutex_t electQuelock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t electCond = PTHREAD_COND_INITIALIZER;

static int MasterThreadVal;
pthread_mutex_t masterlock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t masterCond = PTHREAD_COND_INITIALIZER;

static bool ExitConsumer = false;

static unsigned int SleepCnt = 0;
pthread_mutex_t sleeplock = PTHREAD_MUTEX_INITIALIZER;

static unsigned int ExitCheck = false;
pthread_mutex_t exitlock = PTHREAD_MUTEX_INITIALIZER;

static int * CheackIfProducer;
static int cnt = 0;

#define BUFFERSIZE 100
#define TOALPRODUCTION 1000

vector<int> data_queue;
static int curBufferSize = 0;
static int curtotalProd = 0;

pthread_mutex_t prodlock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t conslock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t prodconselemlock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t prodCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t readCheckIfProd = PTHREAD_MUTEX_INITIALIZER;

void * nThreads(void * thNo) {

	pthread_mutex_lock(&electQuelock);

	int num = 0;
	unsigned int quesize = 0;
	if (election_queue.size() < TotalThread+1) {
		num = rand()%1000000 ;
		election_queue.push_back(num);
		quesize = election_queue.size();
		cout << "Producer thread No.: " << *(int *)thNo << " Queue size: " << quesize << endl;
	}
	if (quesize == TotalThread) {
		int m = INT_MAX;
		unsigned int idx = 0;
		while (idx < TotalThread) {
			cout <<  election_queue[idx] << "  ";
			if (m > election_queue[idx]) {
				m = election_queue[idx];
			}
			++idx;
			MasterThreadVal = m;
		}
		cout << endl;
		pthread_cond_broadcast(&electCond);
	}

	if (quesize < TotalThread) {
		cout << "Waiting thread No. " << *(int *)thNo << endl;
		pthread_cond_wait(&electCond, &electQuelock);
	}
	pthread_mutex_unlock(&electQuelock);

	/*Masrer code start here*/
	pthread_mutex_lock(&masterlock);

	if (num == MasterThreadVal)  {

		*(CheackIfProducer+(int)*(int *)thNo) = 2; 

		cout << "Master elected thread No: " << *(int *)thNo << " value: "  << num << endl;
		int j = 0;
		while (j < TotalProducer) {
			int p = rand()%TotalThread;
			if (p != (int)*(int *)thNo and *(CheackIfProducer+p) != 1) {
				*(CheackIfProducer+p) = 1;
				++j;
				cout << "Master make me producer " << p << " " << j << endl;
			}
		}
		cout << "Master No: " << *(int *)thNo <<  " finished "<< endl;

		pthread_mutex_lock(&sleeplock);
		SleepCnt = SleepCnt+1;
		pthread_mutex_unlock(&sleeplock);

		if (SleepCnt == TotalThread){
			cout << "BroadCasting master : " << *(int *)thNo << endl;
			pthread_cond_broadcast(&masterCond);
		}
		else {
			cout << "SleepCnt: " <<  SleepCnt << endl;
			cout << "Waiting master : " << *(int *)thNo << endl;
			pthread_cond_wait(&masterCond, &masterlock);

		}

		while (ExitConsumer == false) {/*cout << "C E \n"*/;}

		// pthread_exit(NULL);
	}
	if (num != MasterThreadVal) {

		pthread_mutex_lock(&sleeplock);
		SleepCnt = SleepCnt+1;
		pthread_mutex_unlock(&sleeplock);

		if (SleepCnt == TotalThread){
			cout << "BroadCasting for master to assign post : " << *(int *)thNo << endl;
			pthread_cond_broadcast(&masterCond);
		}
		else {
			cout << "SleepCnt: " <<  SleepCnt << endl;
			cout << "Waiting for master to assign post : " << *(int *)thNo << endl;
			pthread_cond_wait(&masterCond, &masterlock);
		}
	}
	pthread_mutex_unlock(&masterlock);

	/*Producer's code*/

	cout << "Checking if as Producer: " << *(int *)thNo << endl;
	fflush(stdin);
	// pthread_mutex_lock(&readCheckIfProd);
	if (*(CheackIfProducer+(int)*(int *)thNo) == 1) {
		// pthread_mutex_unlock(&readCheckIfProd);
		cnt = cnt +1;
		cout << "Assign as Producer: " << *(int *)thNo << endl;
		fflush(stdin);

		while (1) {
			// sleep(5);

			pthread_mutex_lock(&prodlock);
			while (curBufferSize == BUFFERSIZE) {
				cout << "Wating for consumer to response : " << *(int *)thNo << endl;
				fflush(stdin);

				if (ExitCheck == true) break;
				else pthread_cond_wait(&prodCond, &prodlock);

			}
			cout << "Producer " << curBufferSize  << " " << curtotalProd << " " << *(int *)thNo << endl;
			fflush(stdin);


			if (curtotalProd == TOALPRODUCTION) {

				pthread_mutex_lock(&exitlock);
				ExitCheck = true;
				ExitConsumer = true;
				pthread_mutex_unlock(&exitlock);


				pthread_cond_broadcast(&consCond);
				pthread_cond_broadcast(&prodCond);
				pthread_mutex_unlock(&prodlock);
				cout << "Thread No: " << *(int *)thNo <<  " finished "<< endl;
				fflush(stdin);


				// pthread_exit(NULL);
				break;
			}

			pthread_mutex_lock(&prodconselemlock);

			int data = rand()%1000 ;
			data_queue.push_back(data);
			curBufferSize = data_queue.size();
			curtotalProd = curtotalProd+1;
			usleep((rand()%3)*(TotalThread/TotalProducer));

			pthread_mutex_unlock(&prodconselemlock);

			if (curBufferSize >= 1) pthread_cond_broadcast(&consCond);

			if (curtotalProd == TOALPRODUCTION) {

				pthread_mutex_lock(&exitlock);
				ExitCheck = true;
				ExitConsumer = true;
				pthread_mutex_unlock(&exitlock);

				pthread_cond_broadcast(&prodCond);
				pthread_cond_broadcast(&consCond);
				pthread_mutex_unlock(&prodlock);
				cout << "Thread No: " << *(int *)thNo <<  " finished "<< endl;
				fflush(stdin);


				// pthread_exit(NULL);
				break;
			}

			pthread_mutex_unlock(&prodlock);
		}
	}
	// pthread_mutex_unlock(&readCheckIfProd);
	/*Consumer's Code*/
	// pthread_mutex_lock(&readCheckIfProd);
	// cout << "Checking if as Consumer: " << *(int *)thNo << endl;
	// fflush(stdin);

	else if (*(CheackIfProducer+(int)*(int *)thNo) == 0) {
		// pthread_mutex_unlock(&readCheckIfProd);
		cnt = cnt +1;
		cout << "Assign as Consumer: " << *(int *)thNo << endl;
		fflush(stdin);

		while(1) {
			// sleep(5);
			pthread_mutex_lock(&conslock);

			while (curBufferSize == 0) {
				cout << "Wating for producer to response : " << *(int *)thNo << endl;
				fflush(stdin);
				if (ExitCheck == true) break;
				else pthread_cond_wait(&consCond, &conslock);

			}

			cout << "Consumer " << curBufferSize  << " " << curtotalProd << " " << *(int *)thNo << endl;
			fflush(stdin);


			if (ExitCheck == true) {

				pthread_mutex_lock(&exitlock);
				ExitCheck = true;
				pthread_mutex_unlock(&exitlock);

				pthread_cond_broadcast(&prodCond);
				pthread_cond_broadcast(&consCond);
				pthread_mutex_unlock(&conslock);

				cout << "Thread No: " << *(int *)thNo <<  " finished "<< endl;
				fflush(stdin);
				// pthread_exit(NULL);
				break;
			}

			if (curBufferSize < BUFFERSIZE) pthread_cond_broadcast(&prodCond);

			pthread_mutex_lock(&prodconselemlock);

			if (!data_queue.empty()) data_queue.pop_back();
			curBufferSize = data_queue.size();
			usleep((3*TotalThread)/TotalProducery);

			pthread_mutex_unlock(&prodconselemlock);


			if (ExitCheck == true) {

				pthread_mutex_lock(&exitlock);
				ExitCheck = true;
				pthread_mutex_unlock(&exitlock);

				pthread_cond_broadcast(&prodCond);
				pthread_cond_broadcast(&consCond);
				pthread_mutex_unlock(&conslock);

				cout << "Thread No: " << *(int *)thNo <<  " finished "<< endl;
				fflush(stdin);
				// pthread_exit(NULL);
				break;
			}

			pthread_mutex_unlock(&conslock);
		}
	}
	else {
		cout << "Finding What the heck is: " << *(int *)thNo << endl;
		cout << cnt  << "*-----------------------------------------------------------------------------------------------------*" << endl;
		// return NULL;
	}
	// pthread_mutex_unlock(&readCheckIfProd);
	cout << cnt  << "*------------------------------------------*-----*----------------------------------------------------*" << endl;
	return NULL;
}

int main(int argc, char const *argv[])
{
	int n, p;
	cout << "Enter N and P" << endl;
	cin >> n;
	cin >> p;
	cout << n << " " << p << endl;
	if (p <= 0 && n <= 0) {
		cout << "Incorrect Input" << endl;
		exit(0);
		return 0;
	}
	if (p >= (n-1)) {
		cout << "Incorrect Input" << endl;
		exit(0);
		return 0;
	}

	TotalThread = n;
	TotalProducer = p;
	MasterThreadVal = 0;

	int argu[n];

	CheackIfProducer = (int *)malloc(sizeof(int)*n);

	pthread_t * threads;
	threads = (pthread_t *)malloc(sizeof(pthread_t)*n);

	clock_t time_req; 
	time_req = clock();
	
	ofstream myfile;
  	myfile.open ("example.txt", std::ofstream::out | std::ofstream::app);
	cout << "Program started running" << endl;
	for (int i = 0; i < n; ++i) {
		argu[i] = i;
		*(CheackIfProducer+i) = 0;
		pthread_create(threads+i, NULL, nThreads, &argu[i]);

	}
	for (int i = 0; i < n; ++i) {
		pthread_join(*(threads+i), NULL);
	}
	time_req = clock()- time_req; 
	myfile << p << " " << (float)time_req/CLOCKS_PER_SEC << endl;

	return 0;
}