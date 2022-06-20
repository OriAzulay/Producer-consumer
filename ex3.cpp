//Ori Azulay 206336794

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <queue>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <pthread.h>
#include <vector>

using namespace std;
/*global variables:*/
//queue array
queue<string> * queueLIst;
//queue capacity
int N = 10;

// This queue is FIFO : in ->>|4|3|2|1|-->out
class BoundedQueue:public queue<string>{

	mutex m;
	size_t Qsize;
	public:
	BoundedQueue(size_t capacity){
		this->Qsize = capacity;
	}
	void push(string s){
		m.lock();
		//full
		if(this->size() != this->Qsize){
			cout<<"log: push: "<<s<<endl;
			queue::push(s);
		m.unlock();
		}
	}
	void pop(){
		m.lock();
		if(!this->empty()){
			cout<<"log: pop: "<<this->front()<<endl;
			queue::pop();
		}
		m.unlock();
	}
	size_t getSize(){
		return Qsize;
	}

};

void * producer(void* param){
	//TODO : create string for Q here
	int id = *(int*)param;
	BoundedQueue* q = new BoundedQueue(N);
	//----test
	q->push("producer number: " + to_string(id));
	//---
	queueLIst[id] = *q;
	return NULL;	
}

void Producer(int id){
	BoundedQueue* q = new BoundedQueue(N);
	queueLIst[id] = *q;
}

int main()

{
	cout<<"Start"<<endl;
	int N = 10;
	/*pthread_t*  t;
	for(int i=0; i<N; i++){
		int* a = (int*)malloc(sizeof(int)); // for passing a real value and not address of register
		*a = i;
		cout<<"log: create producer number"<<*a<<endl;
		//TODO: to see how insert queue size for each producer
		int temp = pthread_create(&t[i],NULL,producer,a);
	}
	for(int i=0;i<queueLIst->size();i++){
		cout<<queueLIst[i].front()<<endl;
	}*/
	
	for (int i = 0; i < N ;  i++) {
        std::thread t(Producer, i + 1);
    }
	for(int i=0;i<queueLIst->size();i++){
		cout<<queueLIst[i].front()<<endl;
	}
    
}