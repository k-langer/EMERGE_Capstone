#include <iostream>
using namespace std;
#include <pthread.h>
#include <unistd.h>

class Smoothing {
		double data;
		double Data;
		const static int time_scale = 10; 
		pthread_t t1;
		double * Ary; 
		int Len;  
	public:
		double get() { return data; }
		void update(double);
		void reset(double D) {
			data = D;
			Data = D;  
		}
		void * do_update_in_thread() {
			double dec = (Data-data)/time_scale; 
			double max = (Data > data) ? Data : data; 
			double min = (Data < data) ? Data : data;  	
			while (max >= min) {	
				data += dec;
				(dec > 0) ? min += dec : max += dec; 
				usleep(time_scale/10); 
			}
		}
		void * keep_buffer_full() {
			int i = 0; 
			while (1) {
				Ary[i] = data; 
				usleep(time_scale/10); 
				i = (i+1) % Len;  
			}
		}

		pthread_t buffer_fill(double* , int);
		/* C++ 2011 needed
		string toString() {
			return "" + data; 
		}
		*/
};
static void* update_in_thread(void * o) {
	return ((Smoothing *) o)->do_update_in_thread(); 
}

void Smoothing::update (double D) {
	Data = D; 
	if ( t1 ) {
		pthread_cancel(t1);
	}
	pthread_create(&t1, NULL, update_in_thread, this);
}
static void* buffer_full(void * o) {
	return ((Smoothing *) o)->keep_buffer_full();
}

pthread_t Smoothing::buffer_fill(double* ary, int len) {
	pthread_t t2;
	Len = len;
	Ary = ary; 
	for (int i = 0; i < len; i++) {
		ary[i] = data; 
	}
	pthread_create(&t1, NULL, buffer_full, this);  
	return t2;
}
int main() {
	Smoothing c; 
	c.reset(1000); 
	cout << c.get() << endl;
	c.update(10);	
	usleep(5000);
	cout << c.get() << endl;
	c.update(2000); 
	usleep(5000);
	cout << c.get() << endl; 
	return 0;
}


