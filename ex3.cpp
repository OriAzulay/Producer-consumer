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
#include <semaphore.h>

using namespace std;
/*global variables:*/
vector<queue<string>> queueLIst;//queue array
int N = 10;//queue capacity

// This queue is FIFO : in ->>|4|3|2|1|-->out
class Bounded_Buffer:public queue<string>{
	sem_t* Sfull;
	sem_t* Sempty;
	
	mutex m;
	size_t Qsize;
	public:
	Bounded_Buffer(size_t capacity){
		this->Qsize = capacity;
		//sem_init(Sfull,0,0);
		//sem_init(Sempty,0,capacity);
	}
	void insert(string s){
		//sem_wait(Sempty);
		m.lock();
		//full
		if(this->size() != this->Qsize){
			queue::push(s);
		m.unlock();
		//sem_post(Sfull);
		}
	}
	string remove(){
		//sem_wait(Sfull);
		m.lock();
		string firstObj = NULL;
		if(!this->empty()){
			firstObj = queue::front();
			queue::pop();
		}
		m.unlock();
		//sem_post(Sempty);
		return firstObj;
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
	string s;
	switch (i % 3)
	{
	case 0:
		s = " NEWS ";
		break;
	case 1:
		s =  " SPORTS ";
		break;
	case 2: 
		s = " WEATHER ";

	}
	return s;
}


void * producer(void* param){
	//TODO : create string for Q here
	params p = *(params*)param;
	Bounded_Buffer* q = new Bounded_Buffer(N);
	p.numS = 3;
	mutex p_m;
	for(int i=0; i<p.numS; i++){
		p_m.lock();
		string product = "producer " + to_string(p.id) + createNews(p.id) + to_string(i);
		sleep(0.01);
		q->insert(product);
		p_m.unlock();
	}
	q->insert("DONE");
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
		int* a = (int*)malloc(sizeof(int));
		*a = i;
		p.id = *a;
		pthread_t  t;
		int temp = pthread_create(&t,NULL,producer,&p);
	}
	for(int i=0;i<queueLIst.size();i++){
		for(int j=0; j<3; j++){
			cout<<queueLIst[i].front()<<endl;
		}
		
	}
}