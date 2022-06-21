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
#include <iterator>

using namespace std;
/*global variables:*/
vector<queue<string>> queueLIst;//queue array
int N = 3;//queue capacity
//global unbounded queue (implement unboubded inide despetcher & unBounded_BUffer class)
queue<string> sportQ;
queue<string> newsQ;
queue<string> wheaterQ;

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
class UnBounded_Buffer:public queue<string>{
	sem_t S;
	mutex m;
	size_t Qsize;
	public:
	UnBounded_Buffer(size_t capacity){
		this->Qsize = capacity;
		sem_init(&S,0,capacity);
	}
	void insert(string s){
		m.lock();
		queue::push(s);
		m.unlock();
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
struct coEditParam
	{
		Bounded_Buffer* screen = new Bounded_Buffer(N);
		int i;
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


void* producer(void* param){
	params p = *(params*)param;
	Bounded_Buffer* q = new Bounded_Buffer(N);
	mutex p_m;
	int i;
	for(i=0; i<p.numS; i++){
		string s = to_string(i+1);
		p_m.lock();
		string product = "producer " + to_string(p.id) + createNews() + s;
		p_m.unlock();
		q->insert(product);
		sleep(0.1);
	}
	q->insert("DONE");
	queueLIst.push_back(*q);
	return NULL;	
}

void* dispatcher(void* param){
	vector<queue<string>> copyQ = queueLIst;
	mutex m;
	sem_t s;
	sem_init(&s,0,N);
	while(!copyQ.empty()){
		vector<queue<string>>::iterator it = copyQ.begin();
		for(it; it != copyQ.end() ; it++){
			if(copyQ.empty()){
				break;
			}
			if(!(*it).empty()){
				switch ((*it).front()[11])
				{
				case 'S':
					sportQ.push((*it).front());
					cout<<"SPORT Inserted to the 'S dispatcher queue' \n";//not sure if mendatory
					break;
				case 'N':
					newsQ.push((*it).front());
					cout<<"NEWS Inserted to the 'N dispatcher queue' \n";//not sure if mendatory
					break;
				case 'W':
					wheaterQ.push((*it).front());
					cout<<"WHEATER Inserted to the 'W dispatcher queue' \n";//not sure if mendatory
					break;
				default: 
					break;
				}
				// for bounded queue
				sem_wait(&s); 
				(*it).pop();
				sem_post(&s);
			}else{
				copyQ.erase(it);
			}
		}
	}		
	return NULL;
}

void* CoEditor(void* param){
	queue<string> sportQtemp = sportQ;
	queue<string> newsQtemp = newsQ;
	queue<string> wheaterQtemp = wheaterQ;
	coEditParam p = *(coEditParam*)param;
	switch (p.i)
	{
	case 1:
		for(int i=0;i<sportQ.size(); i++){
			p.screen->insert(sportQtemp.front());
			sportQtemp.pop();
		}
		
		break;
	case 2:
		for(int i=0;i<newsQ.size(); i++){
			p.screen->insert(newsQtemp.front());
			newsQtemp.pop();
		}
		break;
	case 3:
		for(int i=0;i<wheaterQ.size(); i++){
			p.screen->insert(wheaterQtemp.front());
			wheaterQtemp.pop();
		}
		break;
	default:
		break;
	}
	return NULL;
}



int main()
{
	cout<<"Start"<<endl;
	pthread_t t[N];
	//generae rabdom for queue sizes
	random_device r;
	mt19937 gen(r());
	uniform_int_distribution<> dist(1,4);
	//--------producers-------------
	for(int i=0; i<N; i++){
		params p;
		int* a = (int*)malloc(sizeof(int));
		*a = i;
		p.id = *a + 1;
		p.numS = dist(gen);
		pthread_create(&t[i],NULL,producer,&p);
		
	}
	//-------------------------------

	//init dispatcher queues:
	pthread_t dispatch;
	pthread_create(&dispatch,NULL,dispatcher,NULL);
	//init coEditor :
	pthread_t coEdit[3];
	coEditParam p;
	for(int i=0; i<3; i++){
		int* a = (int*)malloc(sizeof(int));
		*a = i;
		p.i = *a;
		pthread_create(&coEdit[i],NULL,CoEditor,&p);
	}

	for(int k=0; k<N; k++){
		pthread_join(t[k],NULL);
	}
	pthread_join(dispatch,NULL);

	for(int k=0; k<3; k++){
		pthread_join(coEdit[k],NULL);
	}
	
//TEST PRINTS
	for(int i=0;i<N;i++){
		cout<<queueLIst.at(i).size()<<endl;
		while(queueLIst.at(i).front() != "DONE"){
			cout<<queueLIst.at(i).front()<<endl;
			queueLIst.at(i).pop();
			cout<<"coEditor is: "<<p.screen->remove();
			
		}		
	}
}