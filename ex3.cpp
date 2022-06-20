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
vector<queue<string>> queueLIst;//queue array
int N = 10;//queue capacity

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

string createNews(int i){
	switch (i % 3)
	{
	case 1:
		return " NEWS ";
		break;
	case 2:
		return " SPORTS ";
		break;
	case 3:
		return " WEATHER ";

	}
	return NULL;
}


void * producer(void* param){
	//TODO : create string for Q here
	params p = *(params*)param;
	BoundedQueue* q = new BoundedQueue(N);
	string product = "produce " + to_string(p.id) + createNews(p.id) + to_string(p.numS);
	//----test
	q->push(product);
	//---
	queueLIst.push_back(*q);
	return NULL;	
}

//TODO: to see how insert queue size for each producer
int main()
{
	cout<<"Start"<<endl;
	int N = 10; //size of producers
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