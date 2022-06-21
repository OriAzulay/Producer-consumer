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
#include <random>

using namespace std;
/*global variables:*/
vector<queue<string>> queueLIst;//queue array
int N = 10;//queue capacity

// This queue is FIFO : in ->>|4|3|2|1|-->out
class Bounded_Buffer:public queue<string>{
	sem_t S;
	mutex m;
	size_t Qsize;
	public:
	Bounded_Buffer(size_t capacity){
		this->Qsize = capacity;
		sem_init(&S,0,capacity);
	}
	void insert(string s){
		sem_wait(&S);
		m.lock();
		queue::push(s);
		m.unlock();
		sem_post(&S);
	}
	string remove(){
		sem_wait(&S);
		m.lock();
		string firstObj = NULL;
		if(!this->empty()){
			firstObj = queue::front();
			queue::pop();
		}
		m.unlock();
		sem_post(&S);
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

string createNews(){
	string s;
	random_device r;
	mt19937 gen(r());
	uniform_int_distribution<> dist(1,3);

	switch (dist(gen))
	{
	case 1:
		s = " NEWS ";
		break;
	case 2:
		s =  " SPORTS ";
		break;
	case 3: 
		s = " WEATHER ";

	}
	return s;
}


void * producer(void* param){
	//TODO : create string for Q here
	params p = *(params*)param;
	Bounded_Buffer* q = new Bounded_Buffer(N);
	mutex p_m;
	
	int i;
	for(i=0; i<p.numS; i++){
		string s = to_string(i);
		p_m.lock();
		string product = "producer " + to_string(p.id) + createNews() + s;
		p_m.unlock();
		q->insert(product);
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
	pthread_t t[N];
	for(int i=0; i<N; i++){
	// temp queue size generator------
	random_device r;
	mt19937 gen(r());
	uniform_int_distribution<> dist(1,7);
	//--------------------------------
		params p;
		int* a = (int*)malloc(sizeof(int));
		*a = i;
		p.id = *a;
		p.numS = dist(gen);
		int temp = pthread_create(&t[i],NULL,producer,&p);
		sleep(0.5);
		
	}
	for(int k=0; k<N; k++){
		pthread_join(t[k],NULL);
	}
	for(int i=0;i<N;i++){
		cout<<queueLIst.at(i).size()<<endl;
		while(queueLIst.at(i).front() != "DONE"){

			cout<<queueLIst.at(i).front()<<endl;
			queueLIst.at(i).pop();
		}		
	}
}