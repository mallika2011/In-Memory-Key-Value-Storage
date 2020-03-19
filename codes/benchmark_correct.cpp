#include<bits/stdc++.h>
#include <unistd.h> 
#include <stdint.h> 
#include <pthread.h>
#include <time.h>
#include "kvStore.cpp"
using namespace std;

string sliceToStr(Slice& a) {
    string ret = "";

    for (int i = 0; i < a.size; i++)
        ret += a.data[i];

    return ret;
}

void strToSlice(string l, Slice& a) {
    a.size = l.length();
    a.data = (char*)malloc(a.size);
    strncpy(a.data, l.c_str(), a.size);
}

string random_key(int stringLength){
	string k = "";
	string letters = "";
	for(char i = 'a';i<='z';i++)letters+=i;
	for(char i = 'A';i<='Z';i++)letters+=i;
	for(int i=0;i<stringLength;i++)
		k = k + letters[(rand()%52+rand()%10)%52];

	return k;
}

string random_value(int stringLength){
	string v = "";
	string letters = "";
	for(int i = 32;i<127;i++)letters+=char(i);

	for(int i=0;i<stringLength;i++)
		v = v + letters[rand()%95];

	return v;
}

kvStore kv(10000000);
map<string,string> db;
long long db_size = 0;

long CLOCKS_PER_SECOND = 1000000;

void *myThreadFun(void *vargp) 
{ 
	int transactions=0;
	clock_t start = clock();
	int time = 10;
	clock_t tt = clock();
	while((float(tt-start)/CLOCKS_PER_SECOND)<=time)
	{

		for(int i=0;i<10000;i++)
		{
			transactions+=1;
			int x = rand()%5;
			if(x==0)
			{
				string key = random_key(rand()%64 + 1);
				Slice s_key,s_value;
				strToSlice(key,s_key);
				bool ans = kv.get(s_key,s_value);
			}
			else if(x==1)
			{
				string key = random_key(rand()%64 + 1);
				string value = random_value(rand()%255 + 1);
				Slice s_key,s_value,temp;
				strToSlice(key,s_key);
				strToSlice(value,s_value);

				bool check = kv.get(s_key,temp);
				bool ans = kv.put(s_key,s_value);

				if(check == false) {
					pthread_mutex_lock(&db_lock);
					db_size++;
					pthread_mutex_unlock(&db_lock);

				}
			}
			else if(x==2)
			{
				int temp=db_size;
				if (temp == 0)
					continue;		
				int rem = rand()%temp;
				Slice s_key,s_value;
				bool check = kv.get(rem,s_key,s_value);
				check = kv.del(s_key);
				pthread_mutex_lock(&db_lock);

				db_size--;
					pthread_mutex_unlock(&db_lock);

			}
			else if(x==3)
			{
				int temp=db_size;
				if (temp == 0)
					continue;
				int rem = rand()%temp;
				Slice s_key,s_value;
				bool check = kv.get(rem,s_key,s_value);
			}
			else if(x==4)
			{
				int temp=db_size;
				if (temp == 0)
					continue;
				int rem = rand()%temp;
				bool check = kv.del(rem);
					pthread_mutex_lock(&db_lock);

				db_size--;
					pthread_mutex_unlock(&db_lock);

			}
		}
		tt=clock();
	}
	cout<<transactions/time<<endl;
	return NULL;  
} 

int main()
{
	int put_enteries = 100000;
	vector<Slice> keys;
	vector<Slice> values;
	for(int i=0;i<put_enteries;i++)
	{
		string key = random_key(rand()%63 + 2);
		string value = random_value(rand()%254 + 2);
		db[key] = value;
		Slice k,v;
		strToSlice(key,k);
		strToSlice(value,v);
		keys.push_back(k);
		values.push_back(v);
		db_size = db.size();
	}

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	long double st = ts.tv_nsec / (1e9) + ts.tv_sec;
	for(int i=0;i<put_enteries;i++)
		kv.put(keys[i],values[i]);
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    long double en = ts.tv_nsec / (1e9) + ts.tv_sec;
	long double duration = en-st;

	cout<<"TIME FOR PUTTING "<<put_enteries<<" ENTERIES = "<<(duration)<<" SEC "<<endl;

	bool incorrect = false;

	for(int i=0;i<10000;i++)
	{
		cout<<i<<endl;
		int x = rand()%5;
		if(x==0)
		{
			string key = random_key(rand()%63 + 2);
			Slice s_key,s_value;
			strToSlice(key,s_key);
			bool ans = kv.get(s_key,s_value);
			map<string,string>:: iterator itr = db.find(key);
			if((ans==false && itr != db.end()) || (ans==true && itr->second != sliceToStr(s_value) ))
				incorrect = true;
		}
		else if(x==1)
		{
			int k = rand()%63 + 2;
			int v = rand()%254 + 2;
			string key = random_key(k);
			string value = random_value(v);
			db[key] = value;
			Slice s_key,s_value;
			strToSlice(key,s_key);
			strToSlice(value,s_value);
			bool ans = kv.put(s_key,s_value);

			Slice check;
			bool check2 = kv.get(s_key,check);
			db_size = db.size();
			if(check2 == false || value != sliceToStr(check))
				incorrect = true;
		}
		else if(x==2)
		{
			int rem = rand()%db_size;
			map<string,string>:: iterator itr = db.begin();
			advance(itr,rem);
			string key = itr->first;
			Slice s_key,s_value;
			strToSlice(key,s_key);
			bool check = kv.del(s_key);
			db.erase(itr);
			db_size--;

			bool check2 = kv.get(s_key,s_value);
			if(check2 == true)
				incorrect = true;
		}
		else if(x==3)
		{
			int rem = rand()%db_size;
			Slice s_key,s_value;
			bool check = kv.get(rem,s_key,s_value);
			map<string,string>:: iterator itr = db.begin();
			for(int i=0;i<rem;i++)itr++;
			if( itr->first != sliceToStr(s_key) || itr->second != sliceToStr(s_value))
				incorrect = true;
		}
		else if(x==4)
		{
			int rem = rand()%db_size;
			map<string,string>:: iterator itr = db.begin();
			for(int i=0;i<rem;i++)itr++;
			string key = itr->first;
			bool check = kv.del(rem);
			db.erase(itr);
			db_size--;
			Slice s_key,s_value;
			strToSlice(key,s_key);
			bool check2 = kv.get(s_key,s_value);
			if(check2 == true)
				incorrect = true;
		}
	}
	if(incorrect == true)
		cout<<"INCORRECT OUTPUT"<<endl;
	else
		cout<<"CORRECT OUTPUT"<<endl;

	int threads = 4;

	pthread_t tid[threads];
	for (int i = 0; i < threads; i++) 
	{
		tid[i] = i;
        pthread_create(&tid[i], NULL, myThreadFun, (void *)&tid[i]); 
	}
	for(int i=0;i<threads;i++)
		pthread_join(tid[i],NULL);
	return 0;
}