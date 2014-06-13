// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <unordered_map>
#include <time.h>
#include <iostream>
#include "windows.h"
#include <exception>

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <algorithm>
#include <vector>
#include <memory>
#include <set>
#include <map>

enum TYY
{
	VTYPE_INT32,
	VTYPE_INT64,
	VTYPE_STRING,
	VTYPE_BINARY,
	VTYPE_END,
};

union VaData
{
	int32_t int32_data;
	int64_t int64_data;
	char*	p_str_data;
	void*	p_bin_data; 
};

struct VaNode
{
	VaData data;
	int32_t data_capacity;
	int32_t data_type;

};

#define NodeSize  sizeof(VaNode)

class Valist
{
public:
	VaNode* P_Vadata;
	int size;
	int Capacity;
	bool fixed_size;
	//bool fixed_string;
	
	Valist(int nsize, bool bfixed_size = false)
	{
		fixed_size = bfixed_size;
		P_Vadata = (VaNode*)malloc(nsize * NodeSize);
		size = 0;
		Capacity = nsize;
	}

	Valist(const Valist& c)
	{
		fixed_size = c.fixed_size;
		P_Vadata = (VaNode*)malloc(c.Capacity * NodeSize);
		memcpy(P_Vadata, c.P_Vadata, c.size * NodeSize);
		for (int i = 0; i < c.size; i++)
		{
			if (c.P_Vadata[i].data_type == VTYPE_STRING)
			{
				P_Vadata[i].data.p_str_data = (char*)malloc(c.P_Vadata[i].data_capacity);
				memcpy(P_Vadata[i].data.p_str_data, c.P_Vadata[i].data.p_str_data, c.P_Vadata[i].data_capacity);
				P_Vadata[i].data_capacity = c.P_Vadata[i].data_capacity;
			}
		}
		size = c.size;
		Capacity = c.Capacity;
	}

	Valist(Valist&& c)
	{
		fixed_size = c.fixed_size;
		int n = NodeSize;
		P_Vadata = c.P_Vadata;
		size = c.size;
		Capacity = c.Capacity;
		c.P_Vadata = NULL;
		c.size = 0;
		c.Capacity = 0;
	}

	Valist& operator=(const Valist& c)
	{
		for (int i = 0; i < size; i++)
		{
			if (P_Vadata[i].data_type == VTYPE_STRING)
			{
				free(P_Vadata[i].data.p_str_data);
			}
		}

		if (P_Vadata != NULL)
		{
			free(P_Vadata);
		}
		P_Vadata = NULL;

		fixed_size = c.fixed_size;
		P_Vadata = (VaNode*)malloc(c.Capacity * NodeSize);
		memcpy(P_Vadata, c.P_Vadata, c.size * NodeSize);
		for (int i = 0; i < c.size; i++)
		{
			if (c.P_Vadata[i].data_type == VTYPE_STRING)
			{
				P_Vadata[i].data.p_str_data = (char*)malloc(c.P_Vadata[i].data_capacity);
				memcpy(P_Vadata[i].data.p_str_data, c.P_Vadata[i].data.p_str_data, c.P_Vadata[i].data_capacity);
				P_Vadata[i].data_capacity = c.P_Vadata[i].data_capacity;
			}
		}
		size = c.size;
		Capacity = c.Capacity;
		return *this;
	}

	Valist& operator=(Valist&& c)
	{
		//fixed_size = c.fixed_size;
		int n = NodeSize;
		VaNode* tmp_p = P_Vadata;
		size_t tmp_size = size;
		size_t tmp_cap = Capacity;
		P_Vadata = c.P_Vadata;
		size = c.size;
		Capacity = c.Capacity;
		c.P_Vadata = tmp_p;
		c.size = tmp_size;
		c.Capacity = tmp_cap;
	}

	void clear()
	{
		for (int i = 0; i < size; i++)
		{
			if (P_Vadata[i].data_type == VTYPE_STRING)
			{
				free(P_Vadata[i].data.p_str_data);
			}
		}
		size = 0;
	}

	~Valist()
	{
	
		for (int i = 0; i < size; i++)
		{
			if (P_Vadata[i].data_type == VTYPE_STRING)
			{
				free(P_Vadata[i].data.p_str_data);
			}
		}

		if (P_Vadata != NULL)
		{
			free(P_Vadata);
		}
		
	}


	bool AddString(const char* str)
	{
		if (size >= Capacity)
		{
			ExtendCapacity();
		}

		size_t need_size = strlen(str) + 1; 
		int index = size;
		char* t= (char*)malloc(need_size);
		P_Vadata[index].data.p_str_data = t; 
		memcpy(P_Vadata[index].data.p_str_data, str, need_size);
		P_Vadata[index].data_capacity = (unsigned short)need_size;
		P_Vadata[index].data_type = VTYPE_STRING;
		size++;
		return true;
	}

	bool AddInt32(int32_t value_int32)
	{
		if (size >= Capacity)
		{
			ExtendCapacity();
		}
		int index = size;
		P_Vadata[index].data.int32_data = value_int32;
		P_Vadata[index].data_type = VTYPE_INT32;
		size++;
		return true;
	}
	bool SetInt32(int index, int32_t data_int32)
	{
		if (index >= size )
		{
			return false;
		}

		if (P_Vadata[index].data_type != VTYPE_INT32)
		{
			return false;
		}
		P_Vadata[index].data.int32_data = data_int32;
		
		return true;
	}

	bool AddInt64(int64_t value_int64)
	{
		if (size >= Capacity)
		{
			ExtendCapacity();
		}
		int index = size;
		P_Vadata[index].data.int64_data = value_int64;
		P_Vadata[index].data_type = VTYPE_INT64;
		size++;
		return true;
	}

	bool SetInt32(int index, int64_t data_int64)
	{
		if (index >= size )
		{
			return false;
		}

		if (P_Vadata[index].data_type != VTYPE_INT64)
		{
			return false;
		}
		P_Vadata[index].data.int64_data = data_int64;

		return true;
	}

	void ExtendCapacity() 
	{
		VaNode* temp = (VaNode*)malloc(2 * Capacity * NodeSize);
		memcpy(temp, P_Vadata, size * NodeSize );

		free(P_Vadata);
		P_Vadata = temp;
		Capacity = 2*Capacity;
	}

	bool SetString(int index, const char* pstr)
	{
		if (index >= size )
		{
			return false;
		}
		
		if (P_Vadata[index].data_type != VTYPE_STRING)
		{
			return false;
		}

		size_t need_size = strlen(pstr) + 1;

		if (P_Vadata[index].data_capacity < need_size)
		{
			free(P_Vadata[index].data.p_str_data);
			P_Vadata[index].data.p_str_data = (char*)malloc(need_size);
		}
		memcpy(P_Vadata[index].data.p_str_data, pstr, need_size);
		return true;
	}

};



int sdsd()
{



// 	long t = GetTickCount();
	int j = 1000;
	for (int i = 1; i < 10000000; i++)
	{
		Valist va(8);
		if (j > 1001)
		{
			j = 0;
		}
		va.AddString("dddddddddddddd");
		va.SetString(i, "sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss");
		va.AddInt32(0);
		
		//void* P = malloc(34);
		//free(P);
	}

	for (int i = 0 ; i < 100; i++)
	{
		void* p = malloc(36);
		if (p)
		{
			free(p);
		}
	}


//	std::cout << GetTickCount() - t <<"aaaaaaaaaaaaaa" <<std::endl;

	// 	 srand((unsigned)time(NULL));  
	// 	std::unordered_map<int , std::string > jj;
	// 	 for(int i = 0; i < 10000000; ++i )
	// 	 {
	// 		 int n = rand();
	// 		 char a[100] = {0};
	// 		 jj[n] = itoa(n, a, 10);
	// 	 }
	// 	 std::cout << GetTickCount() - t << std::endl;
	// 	 int key = 0;
	// 
	// 	 for(int i = 0; i < 100000000; ++i )
	// 	 {
	// 
	// 		 auto it = jj.find(i);
	// 		 if(it == jj.end())
	// 		 {
	// 
	// 			 continue;
	// 		 }
	// 		 else
	// 		 {
	// 			 key += it->first;
	// 		 }
	// 	 }
	// 	 std::cout << key << std::endl;
	//std::cout << GetTickCount() - t << std::endl;
	return 0;
}

#define COUNT 1000*10000000

void swap_4( const char* orig, char* target );


void func()
{
	size_t j = 0;
	for (size_t i = 0; i < COUNT; ++i)
	{
		if (j > 1001)
		{
			j = 0;
		}
		int * pInt = (int*)malloc(sizeof(int));
		//swap_4((char*)pInt, (char*)&j);
		free(pInt);
	}
}


static inline int LgFloor(size_t n) {
	int log = 0;
	for (int i = 4; i >= 0; --i) {
		int shift = (1 << i);
		size_t x = n >> shift;
		if (x != 0) {
			n = x;
			log += shift;
		}
	}
	
	return log;
}
static const size_t kMinAlign   = 16;
#define kPageSize 1 << 13
int AlignmentForSize(size_t size) {
	int alignment =	8;
	if (size > 256 * 1024) {
		// Cap alignment at kPageSize for large sizes.
		alignment = 8192;
		
	} else if (size >= 128) {
		// Space wasted due to alignment is at most 1/8, i.e., 12.5%.
		alignment = (1 << LgFloor(size)) / 8;
	} else if (size >= 16) {
		// We need an alignment of at least 16 bytes to satisfy
		// requirements for some SSE types.
		alignment = 16;
	}
	// Maximum alignment allowed is page size alignment.
	if (alignment > kPageSize) {
		alignment = kPageSize;
	}

	size = (size & ~(alignment-1)) + alignment;
	//CHECK_CONDITION(size < kMinAlign || alignment >= kMinAlign);
	//CHECK_CONDITION((alignment & (alignment - 1)) == 0);
	return alignment;
}
void swap_4( const char* orig, char* target )
{
	register uint32_t x = * reinterpret_cast<const uint32_t*>( orig );
	x = ( x << 24 ) | ( ( x & 0xff00 ) << 8 ) | ( ( x & 0xff0000 ) >> 8 ) | ( x >> 24 );
	* reinterpret_cast<uint32_t*>( target ) = x;
}

// A simple LRU cache written in C++
// Hash map + doubly linked list
#include <iostream>
#include <vector>
#include <hash_map>
using namespace std;
//using namespace __gnu_cxx;
__declspec(align(64)) struct Node{
	int key;
	int data;
};




#include<thread>  
#include<iostream>  
using namespace std;  


#include <atomic>


class SpinLock
{
public:
	SpinLock()
	{
		lk = false;
	}
	~SpinLock()
	{

	}
	void lock()
	{
		while(lk.exchange(true, std::memory_order_acquire))
		{
		}
	}
	void unlock()
	{
		lk.store(false, std::memory_order_release);
	}


private:
	atomic<bool> lk;
};

class SpinRWLock
{
public:
	SpinRWLock()
	{
		lk = false;
	}
	~SpinRWLock()
	{

	}
	void lock()
	{
		while(lk.exchange(true, std::memory_order_acquire))
		{
		}
	}
	void unlock()
	{
		lk.store(false, std::memory_order_release);
	}


private:
	atomic<int> lk;
};





#define CACHE_LINE_SIZE 64
template <typename T>
struct LowLockQueue {
private:
	struct Node {
		Node( T* val ) : value(val), next(nullptr) { }
		T* value;
		atomic<Node*> next;
		char pad[CACHE_LINE_SIZE - sizeof(T*)- sizeof(atomic<Node*>)];
	};

	char pad0[CACHE_LINE_SIZE];

	// for one consumer at a time
	Node* first;

	char pad1[CACHE_LINE_SIZE
		- sizeof(Node*)];

	// shared among consumers
	atomic<bool> consumerLock;

	char pad2[CACHE_LINE_SIZE 
		- sizeof(atomic<bool>)];

	// for one producer at a time
	Node* last; 

	char pad3[CACHE_LINE_SIZE 
		- sizeof(Node*)];

	// shared among producers
	atomic<bool> producerLock;

	char pad4[CACHE_LINE_SIZE 
		- sizeof(atomic<bool>)];

public:
	LowLockQueue() {
		first = last = new Node( nullptr );
		producerLock = consumerLock = false;
	}
	~LowLockQueue() {
		while( first != nullptr ) {      // release the list
			Node* tmp = first;
			first = tmp->next;
			delete tmp->value;       // no-op if null
			delete tmp;
		}
	}

	void Produce( const T& t ) {
		Node* tmp = new Node( new T(t) );
		while( producerLock.exchange(true) )
		{ }   // acquire exclusivity
		last->next = tmp;         // publish to consumers
		last = tmp;             // swing last forward
		producerLock = false;       // release exclusivity
	}

	bool Consume( T& result ) {
		while( consumerLock.exchange(true) ) 
		{ }    // acquire exclusivity
		Node* theFirst = first;
		Node* theNext = first-> next;
		if( theNext != nullptr ) {   // if queue is nonempty
			T* val = theNext->value;    // take it out
			theNext->value = nullptr;  // of the Node
			first = theNext;          // swing first forward
			consumerLock = false;             // release exclusivity
			result = std::move(*val);    // now copy it back
			delete val;       // clean up the value
			delete theFirst;      // and the old dummy
			return true;      // and report success
		}
		consumerLock = false;   // release exclusivity
		return false;           // report queue was empty
	}
};

int max = 2;


#define  MAX_BUFFER_SIZE 64 * 1024
#define  MIN_BUFFER_SIZE 4 * 1024


class Buffer
{
public:
	Buffer()
	{
		pBuff = nullptr;
		size = 0;
	}

	Buffer(const char* p, size_t length)
	{
		
		if (length > MAX_BUFFER_SIZE)
		{
			pBuff = nullptr;
			length = 0;
			return;
		}
		pBuff = (char*)malloc(length);
		memcpy(pBuff, p, length);
		size = length;
	}

	Buffer(const Buffer& buf)
	{
		pBuff = (char*)malloc(buf.size);
		memcpy(pBuff, buf.pBuff, buf.size);
		size = buf.size;
	}
	
	Buffer(Buffer&& buf)
	{
		pBuff = buf.pBuff;
		buf.pBuff = nullptr;
		size = buf.size;
		buf.size = 0;
	}

	Buffer& operator = (const Buffer& buf)
	{
		if (pBuff != nullptr)
		{
			free(pBuff);
		}
		pBuff = (char*)malloc(size);

		memcpy(pBuff, buf.pBuff, size);
		size = buf.size;
		return *this;
	}

	Buffer& operator = (Buffer&& buf)
	{
		if (pBuff != nullptr)
		{
			free(pBuff);
		}
		pBuff = buf.pBuff;
		buf.pBuff = nullptr;
		size = buf.size;
		buf.size = 0;
		return *this;
	}

	~Buffer()
	{
		if (pBuff != nullptr)
		{
			free(pBuff);
		}
		size = 0;
	}
	
	bool CoverToValist(Valist& va)
	{
		va.clear();

		return true;
	}

	char* GetBuff()
	{
		return pBuff;
	}
	size_t GetSize()
	{
		return size;
	}

	char* pBuff;
	size_t size;
};

std::atomic<int> ddd = 0;

class test{  
public:  
	void operator()() const{  
		cout<<"operator() "<<*data<<endl;
		while (true)
		{
			static int i = 0;
			Buffer p;
			if(lq->Consume(p))
			{
				//cout<<"    "<< p.GetBuff()<< endl;
				i++;
				ddd++;
			}
			if(ddd>=1000000)
			{
				break;
			}
		}
		cout<<"OVER "<<this_thread::get_id() <<endl;
			
	}  
	test(LowLockQueue<Buffer>* p,int a=10){  
		cout<<"constructor"<<endl;  
		data=new int(a);
		lq = p;
	}  
	int get() const{  
		return *data;  
	}  
	test(const test& one){  
		cout<<"copy constructor"<<endl;  
		data=new int(one.get());//深拷贝，否则段错误
		lq = one.lq;
	} 
	test(test&& one){  
		cout<<"right copy constructor"<<endl;  
		data=one.data;//深拷贝，否则段错误  
		one.data = NULL;
		lq = one.lq;
	}  
	~test()
	{
		delete data;
	}

private: 
	LowLockQueue<Buffer>* lq;
	int* data;  
};  


class test2{  
public:  
	void operator()() const{  
		cout<<"operator() "<<*data<<endl;
		int i = 0;
		while (true)
		{
		 
			while (true)
			{

				char p2[1000] = {"sassssssssssssssssssssssssssss1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"};
		
				shared_ptr<Buffer> p1 (new Buffer( p2, strlen(p2)));
				lq->Produce(p1);
				Sleep(1);
			}
// 			static int i = 0;
// 			shared_ptr<Buffer> p;
// 			if(lq->Consume(p))
// 			{
// 				cout << this_thread::get_id() <<"    "<< p->pBuff<< endl;
// 				i++;
// 				Sleep(10);
// 			}
// 			else
// 			{
// 				if (i == 2000)
// 				{
// 					break;
// 				}
// 				Sleep(10);
// 			}
		}

	}  
	test2(LowLockQueue<shared_ptr<Buffer>>* p,int a=10){  
		cout<<"constructor"<<endl;  
		data=new int(a);
		lq = p;
	}  
	int get() const{  
		return *data;  
	}  
	test2(const test2& one){  
		cout<<"copy constructor"<<endl;  
		data=new int(one.get());//深拷贝，否则段错误
		lq = one.lq;
	} 
	test2(test2&& one){  
		cout<<"right copy constructor"<<endl;  
		data=one.data;//深拷贝，否则段错误  
		one.data = NULL;
		lq = one.lq;
	}  
	~test2()
	{
		delete data;
	}

private: 
	LowLockQueue<shared_ptr<Buffer>>* lq;
	int* data;  
};  



#define MAC(max) #max

int errrrrr = 1;

enum Myenum
{
	sdsddwww,

	max111,
};




int main()
{
	vector<string> vc1 ;
	vc1.resize(max111);
	std::string d = MAC(sdsddwww);
	vc1.push_back(d);
	
	vc1.push_back(MAC(errrrrr));



	{
	char p2[10000] = {"sassssssssssssssssssssssssssss"};
	shared_ptr<Buffer> p (new Buffer( p2, strlen(p2)));
	}
	AlignmentForSize(100*16);
	LowLockQueue<Buffer> lq;
	


// 	std::atomic_flag flag = ATOMIC_FLAG_INIT;
 	test one(&lq);  
 	thread myThread(one);//函数对象被拷贝进线程空间  


	//thread myThread1(one);//函数对象被拷贝进线程空间  
	//thread myThread2(one);//函数对象被拷贝进线程空间  

	//test2 two(&lq);

	//thread myThread3(two);//函数对象被拷贝进线程空间  
	//thread myThread4(two);//函数对象被拷贝进线程空间 
	DWORD WWW = GetTickCount();
	int i = 0;
	while (true)
	{

		char p2[1000] = {"sassssssssssssssssssssssssssss"};
 		sprintf_s(p2, "sssss_%d", i++);
 		Buffer p1 (p2, strlen(p2)+800);
 		lq.Produce(p1);
		if (i >= 1000000)
		{
			break;
		}
		if (i%1000000 == 0)
		{
			//Sleep(20);
		}
		
	}
	cout << GetTickCount() - WWW << endl;
	myThread.join();
	//myThread1.join();
	//myThread2.join();
	cout << GetTickCount() - WWW << endl;
	//myThread4.join();
	//myThread4.join();
	return 0;  

		DWORD tStart, tEnd;

		std::cout<<MAC(ssssss);
		srand(time(NULL));


		hash_map<int, int> map;
		map[9]= 999;
		cout<<map[9]<<endl;
		cout<<map[10]<<endl;
		
	
		vector<int> vc;
		vc.reserve(2000);
		std::cout<< vc.capacity();


		unordered_map<int, int> map1;

		std::map<int, int> map2;
		for (int i = 30000 ; i < 320000 ; i = i++ )
		{
			map2.insert(make_pair(i, i));
			map1.insert(make_pair(i, i));
		}

		printf("/////-----------------------------------------------------------------------------------\n");
		int j = 0;

		int o = 0;
		tStart = GetTickCount();


	
		Node* pc = new Node[16];

		//func();
		for ( int64_t i = 0; i < 200000000; i ++)
		{
			if (tStart > 1000)
			{
				j = rand()%20000+300000;
			}
			//map1.insert(make_pair(j, j));
			for (int k = 0 ; k < 4 ; k++)
			{
				int  index = j%16;
				if (pc[index].key == j)
				{
					o = 1;
				}
				else
				{
					pc[index].key = j;
					auto it = map1.find(j);

					if (it == map1.end())
					{
						o = 0;
						
					}
				}
 				if (k > 0)
 				{
 					continue;
 				}
 				auto it = map1.find(j);
 								//lru_cache1.Put(j ,j);
 								if (it == map1.end())
 								{
 									o = 0;
 				
 								}
								
			}
			
			//if (tStart > 10000)
			//{
				//map1.erase(it);
			//}
			
		}
		tEnd = GetTickCount();

		printf("%lu\n", tEnd - tStart);


		printf("/////-----------------------------------------------------------------------------------\n");
		
		tStart = GetTickCount();
		//func();
		for ( int i = 0; i < 2000000; i ++)
		{
			if (tStart > 1000)
			{
				j = rand()%20000+300000;
			}
			//map2.insert(make_pair(j, j));
 			auto it = map2.find(j);
 			//if (tStart > 10000)
 			//{
 			//	map2.erase(it);
 			//}
			if (it == map2.end())
			{
				j = 0;
			}
		}
		tEnd = GetTickCount();

		printf("%lu\n", tEnd - tStart);




	sdsd();
	tStart = GetTickCount();
	printf("%lu\n", tStart -tEnd);
	//Valist va(8);
	
 	//std::shared_ptr<Valist> p;
// 	if(p.get())
// 	{
// 		int i = 0;
// 	}
// 	char S = VTYPE_INT32; 
// 	int*d;
// 	int n = sizeof(d);
// 	std::cout << n << std::endl;
// 	int n = sizeof(p);
// 
// 	std::vector<Valist> v;
// 	for (int i = 0; i < 16; i++)
// 	{
// 		v.push_back(va);
// 	}
// 	
// 	v.insert(v.begin(), va);
// 	
// 	std::list<int> l;
// 
// 
// 	for (int i = -100000; i < 200000; i++)
// 	{
// 		l.push_back(i);
// 	}
// 	long t = GetTickCount();
// 	for (int i = 0; i < 100; i = i+2)
// 	{
// 		std::list<int>::iterator it = lower_bound(l.begin(), l.end(), i);
// 		l.insert(it, i);
// 	}
// 	
// 	for (int i = 200; i > -100; i = i-20)
// 	{
// 		std::list<int>::iterator it = lower_bound(l.begin(), l.end(), i);
// 		l.insert(it, i);
// 	}
// 
// 	std::cout << GetTickCount() - t << std::endl;
// 	std::list<int> l;
// 	std::unordered_map<int,int> hah_map;
// 	hah_map.bucket_size(10000);
// 
// 	std::vector<int> vc;
// 	srand(time(NULL)+ 222 );
// 	for (int i = 0 ; i < 10000; i++)
// 	{
// 		int k = rand();
// 		hah_map.insert(std::make_pair(k,k));
// 
// 		vc.push_back(k);
// 	}
// 
// 	int64_t t = GetTickCount();
// 	for(int i = 0 ; i < 10000000; i++)
// 	{
// 		int index = rand()%10000;
// 
// 		auto it = hah_map.find(vc[index]);
// 		
// 		
// 	}
// 	std::cout << GetTickCount() - t << std::endl;
// 	t = GetTickCount();
// 	for(int i = 0 ; i < 10000000; i++)
// 	{
// 		int index = rand()%10000;
// 
// 		vc[index] = 0;
// 	}
// 
// 	std::cout << GetTickCount() - t << std::endl;
// 	t = GetTickCount();
// 	for(int i = 0 ; i < 10000000; i++)
// 	{
// 		int index = i%10000;
// 
// 		vc[index] = 0;
// 	}
// 	std::cout << GetTickCount() - t << std::endl;
// 	std::cout << "11111111111";
	//system("pause");
	return 0;
}

