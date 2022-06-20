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
vector<queue<string>> queueLIst;
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

struct params
{
	int id;
	string type;
	int numS;

};

void * producer(void* param){
	//TODO : create string for Q here
	params p = *(params*)param;
	cout<<"producer number :"<< p.id<<endl;
	BoundedQueue* q = new BoundedQueue(N);
	//----test
	q->push("producer number: " + to_string(p.id));
	//---
	queueLIst.push_back(*q);
	return NULL;	
}


//TODO: to see how insert queue size for each producer
int main()
{
	cout<<"Start"<<endl;
	int N = 10;
	for(int i=0; i<N; i++){
		params p;
		p.id = i;
		p.type;
		pthread_t  t;
		int temp = pthread_create(&t,NULL,producer,&p);
	}
	for(int i=0;i<queueLIst.size();i++){
		cout<<queueLIst[i].front()<<endl;
	}
}