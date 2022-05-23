#include <iostream>
#include <thread>
#include <mutex> 
#include <condition_variable>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

using namespace std;

//Declaring some variables/objects required
//===========================================

struct dataL{ //representing our data as a struct
    int id;
    string code;
};
vector<dataL> Buffer;
bool BufferAccessed = false; //wether the buffer is currently being accessed
int limit = 10; //max number of items in buffer
mutex mtx;
mutex rmtx;
unique_lock<mutex> mLock(mtx);
unique_lock<mutex> rLock(rmtx);
condition_variable cv1;
condition_variable cv2;

//===========================================


//Function decleration and implementation
//===========================================

bool emptySpace(){
    return (0<= Buffer.size() <=10);
}

bool itemsAvailable(){
    return (Buffer.size() >= 1);
}

int generateRandomValue(int x){ //generate a rundem number in range 0-x
    return (rand()%x);
}

void write(){ //implementing the functionality of generating a value and writing it to buffer
    dataL n; //create a new container for data
    n.id = generateRandomValue(99); //generate a radnom number for the id and code 
    n.code = to_string(generateRandomValue(9999));
    BufferAccessed = true; //change status to signify that the buffer is in use
    Buffer.push_back(n); //insert new data into buffer
    BufferAccessed = false; //change status again to release buffer
}

void read(){ //implementing the functionality of reading and outputting a value and removing it from buffer 
    dataL n; //create a new container to load the data into
    BufferAccessed = true; //change status to signify that the buffer is in use
    n = Buffer.back(); //place the last item in buffer into container
    Buffer.pop_back(); //remove last piece of data from buffer 
    BufferAccessed = false; //change status again to release buffer
    cout<<"=========================="<<endl; //output the data
    cout<<"id = "<<n.id<<endl;
    cout<<"code = "<<n.code<<endl;
    cout<<"=========================="<<endl;
}

void writer(){ //writer process
    do{
        cv2.wait(mLock,[]{return !BufferAccessed;}); //lock mutex if buffer is accessed
        cv1.wait(rLock, []{return emptySpace();}); 
        write(); //begin writing to buffer
        cv1.notify_all();
        cv2.notify_all(); //release mutex since buffer is no longer accesesd
    }while(true);
}


void reader(){ //reader process
    while(true){
        cv2.wait(mLock,[]{return !BufferAccessed;}); //lock mutex if buffer is accessed
        //if(Buffer.size() >= limit){
        cv1.wait(rLock, []{return itemsAvailable();});
        read(); //begin reading from buffer
        cv1.notify_all();
        //}
        cv2.notify_all(); //release mutex since buffer is no longer accesesd
    }
}

//===========================================


//entrypoint -->
int main(){ 
    srand(time(0)); //seeding the random function with time to generate a psuedorandom number
    thread th1(writer); //create a thread with writer process
    thread th2(reader); //create a thread with writer process
    th1.join(); //join threads with main to prevent premature termination
    th2.join();
    return 0;
}