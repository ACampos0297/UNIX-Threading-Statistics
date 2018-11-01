/*
    Based on original assignment by: Dr. R. Bettati, PhD
    Department of Computer Science
    Texas A&M University
    Date  : 2013/01/31
 */


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <iomanip>

#include <sys/time.h>
#include <cassert>
#include <assert.h>

#include <cmath>
#include <numeric>
#include <algorithm>

#include <list>
#include <vector>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#include "reqchannel.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
using namespace std;

struct UserRequestData
{
	string data;
	int requests;
	BoundedBuffer* buffer;
};

struct WorkerData 
{
	//should include pointer to histogram and pointer to request channel from main
	RequestChannel* channel;
	BoundedBuffer* buff;
	Histogram* hist;
};


void* request_thread_function(void* arg) {
	/*
		Fill in this function.

		The loop body should require only a single line of code.
		The loop conditions should be somewhat intuitive.

		In both thread functions, the arg parameter
		will be used to pass parameters to the function.
		One of the parameters for the request thread
		function MUST be the name of the "patient" for whom
		the data requests are being pushed: you MAY NOT
		create 3 copies of this function, one for each "patient".
	*/
	
	UserRequestData *userData = (UserRequestData*) arg;
	BoundedBuffer *sharedBuffer = (BoundedBuffer*) userData->buffer;
	//push n requests for this user
	for(int i=0; i < userData->requests; i++) 
	{
		sharedBuffer -> push(userData->data);
	}
}

void* worker_thread_function(void* arg) {
    /*
		Fill in this function. 

		Make sure it terminates only when, and not before,
		all the requests have been processed.

		Each thread must have its own dedicated
		RequestChannel. Make sure that if you
		construct a RequestChannel (or any object)
		using "new" that you "delete" it properly,
		and that you send a "quit" request for every
		RequestChannel you construct regardless of
		whether you used "new" for it.
     */

    while(true) {

    }
}

void* stat_thread_function(void* arg) {
    /*
		Fill in this function. 

		There should 1 such thread for each person. Each stat thread 
        must consume from the respective statistics buffer and update
        the histogram. Since a thread only works on its own part of 
        histogram, does the Histogram class need to be thread-safe????

     */

    for(;;) {

    }
}


/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {
    int n = 100; //default number of requests per "patient"
    int w = 1; //default number of worker threads
    int b = 3 * n; // default capacity of the request buffer, you should change this default
    int opt = 0;
    while ((opt = getopt(argc, argv, "n:w:b:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg); //This won't do a whole lot until you fill in the worker thread function
                break;
            case 'b':
                b = atoi (optarg);
                break;
		}
    }

    int pid = fork();
	if (pid == 0){
		execl("dataserver", (char*) NULL);
	}
	else {

        cout << "n == " << n << endl;
        cout << "w == " << w << endl;
        cout << "b == " << b << endl;

		BoundedBuffer requestsBuffer(b);
		
		cout << "CLIENT STARTED "<<endl;
		cout << "Establishing Control Channel ... ";
        RequestChannel *chan = new RequestChannel("control", RequestChannel::CLIENT_SIDE);
		cout << "done." << endl<< flush;	
        
		//Create user arguments to pass to thread
		UserRequestData JohnArgs;
		JohnArgs.data = "data John Smith";
		JohnArgs.requests = n;
		JohnArgs.buffer = &requestsBuffer;
		
		UserRequestData JaneArgs;
		JaneArgs.data = "data Jane Smith";
		JaneArgs.requests = n;
		JaneArgs.buffer = &requestsBuffer; 
		
		UserRequestData JoeArgs;
		JoeArgs.data = "data Joe Smith";
		JoeArgs.requests = n;
		JoeArgs.buffer = &requestsBuffer;
		
		cout << "POPULATING REQUEST BUFFER ... ";
		pthread_t JohnThread;
		pthread_t JaneThread;
		pthread_t JoeThread;
		
		//pass structs to threads and call request thread function
		pthread_create(&JohnThread, NULL, request_thread_function, &JohnArgs);
		pthread_create(&JaneThread, NULL, request_thread_function, &JaneArgs);
		pthread_create(&JoeThread, NULL, request_thread_function, &JoeArgs);
		
		//join threads 
		pthread_join(JohnThread, NULL);
		pthread_join(JaneThread, NULL);
		pthread_join(JoeThread, NULL);
		cout << " done. "<<endl;
		
		chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl; 
        
/*
        cout << "Pushing quit requests... ";
        for(int i = 0; i < w; ++i) {
            request_buffer.push("quit");
        }
        cout << "done." << endl;

	
        chan->cwrite("newchannel");
		string s = chan->cread ();
        RequestChannel *workerChannel = new RequestChannel(s, RequestChannel::CLIENT_SIDE);

        while(true) {
            string request = request_buffer.pop();
			workerChannel->cwrite(request);

			if(request == "quit") {
			   	delete workerChannel;
                break;
            }else{
				string response = workerChannel->cread();
				hist.update (request, response);
			}
        }
        chan->cwrite ("quit");
        delete chan;
        cout << "All Done!!!" << endl; 

		hist.print ();
		*/
    }
}
