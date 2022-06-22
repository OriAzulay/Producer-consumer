//Ori Azulay 206336794
// based on configuration file (txt), that is numbers only
//(option 2 in the assignment file)

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
#include <sstream>

using namespace std;
/*global variables:*/
vector<queue<string>> queueLIst;//queue array
int N = 10;//queue capacity
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
		string firstObj;
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
	int id; //producer id
	string type; // type
	int numS; // number of products inside producer
	int qSize; //producer queue size
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
	Bounded_Buffer* q = new Bounded_Buffer(p.qSize);
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
	//copy queue for iterate and delete
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
				if((*it).front() != "DONE"){ //avoid reaching to null at [11] index
					switch ((*it).front()[11]) //index of N(in NEWS),S(in SPORTS) or W(in WHEATER)
				{
				case 'S':
				sleep(0.1);
					sem_wait(&s);
					m.lock();
					sportQ.push((*it).front());
					cout<<"SPORT Inserted to the 'S dispatcher queue' \n";//not sure if mendatory
					m.unlock();
					sem_post(&s);
					break;
				case 'N':
				sleep(0.1);
					sem_wait(&s);
					m.lock();
					newsQ.push((*it).front());
					cout<<"NEWS Inserted to the 'N dispatcher queue' \n";//not sure if mendatory
					m.unlock();
					sem_post(&s);
					break;
				case 'W':
				sleep(0.1);
					sem_wait(&s);
					m.lock();
					wheaterQ.push((*it).front());
					cout<<"WHEATER Inserted to the 'W dispatcher queue' \n";//not sure if mendatory
					m.unlock();
					sem_post(&s);
					break;
				default: 
					break;
				}
				// for bounded queue
				sem_wait(&s); 
				(*it).pop();
				sem_post(&s);
				}else{
					sem_wait(&s); 
					(*it).pop();
					sem_post(&s);
					break;
				}
			}else{
				copyQ.erase(it);
			}
		}
	}
	sportQ.push("DONE");
	newsQ.push("DONE");
	wheaterQ.push("DONE");		
	return NULL;
}

void* CoEditor(void* param){
	queue<string> sportQtemp = sportQ;
	queue<string> newsQtemp = newsQ;
	queue<string> wheaterQtemp = wheaterQ;
	coEditParam p = *(coEditParam*)param;
	// case of spors, wheater or news
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



int main(int argc, char* argv[])
{
	/*
	Unfortunately, I was unable to set variables for the configuration file
	Then the code will work with encoded numbers
	(I submitted the assignment late (22.6) due to reserve at the army and relocation)
	i would like your considuration ,please.

	N = number of producers
	dist - Create a random queue size for each producer
	 */
	pthread_t t[N];
	//generae rabdom for queue sizes
	random_device r;
	mt19937 gen(r());
	uniform_int_distribution<> dist(1,4);
	//--------producers-------------
	for(int i=0; i<N; i++){
		params p; // parameters for producers - id, type, queue size 
		int* a = (int*)malloc(sizeof(int));
		*a = i;
		p.id = *a + 1;
		p.numS = dist(gen);
		p.qSize = p.numS;
		int th1 = pthread_create(&t[i],NULL,producer,&p);
		sleep(0.5);
	}
	//-------------------------------

	//init dispatcher queues:
	pthread_t dispatch;
	int th2 = pthread_create(&dispatch,NULL,dispatcher,NULL);
	//init coEditor :
	pthread_t coEdit[3];
	coEditParam p; //parameters for coEditor : id (1,2,3) and bounded queue
	int tempScreenSIze = 50;
	Bounded_Buffer* screenMAnager = new Bounded_Buffer(tempScreenSIze);
	for(int i=0; i<=3; i++){
		int* a = (int*)malloc(sizeof(int));
		*a = i;
		p.i = *a;
		p.screen = screenMAnager;
		int th3 = pthread_create(&coEdit[i],NULL,CoEditor,&p);
	}
	//JOIN
	for(int k=0; k<N; k++){
			pthread_join(t[k],NULL);
		}
		pthread_join(dispatch,NULL);

		for(int k=0; k<3; k++){
			pthread_join(coEdit[k],NULL);
		}
	
	//Sycreen manager
	for(int i=0;i<5*N;i++){
		while(!p.screen->empty()){
			cout<<p.screen->remove()<<endl;
			sleep(1.3);
		}
	}
	cout<<"screen manager DONE";
	
}