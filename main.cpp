#include <iostream>
#include <thread>
#include <mutex> 
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include<condition_variable>

using namespace std;

int generateValue(int x){
    return (rand()%x);
}

struct dataL{
    int id;
    string code;
};

bool accesesd = false;

mutex mtx;
mutex rmtx;

std::unique_lock<std::mutex> rLock(rmtx);
std::unique_lock<std::mutex> mLock(mtx);

std::condition_variable cv1;
std::condition_variable cv2;


std::vector<dataL> Buffer;
int limit = 10;

void write(){
    accesesd = true;
    dataL n;
    n.id = generateValue(99);
    n.code = to_string(generateValue(9999));
    Buffer.push_back(n);
    accesesd = false;
}

bool emptySpace(){
    return (0<= Buffer.size() <=10);
}
bool itemsAvailable(){
    return (Buffer.size() >= 1);
}

void writer(){
    bool es = emptySpace();
    do{
        cv2.wait(mLock,[]{return !accesesd;});
        cv1.wait(rLock, []{return emptySpace();});
        write();
        cv1.notify_all();
        cv2.notify_all();
    }while(true);
}

void read(){
    accesesd = true;
    dataL n;
    n = Buffer.back();
    Buffer.pop_back();
    cout<<"=========================="<<endl;
    cout<<"id = "<<n.id<<endl;
    cout<<"code = "<<n.code<<endl;
    cout<<"=========================="<<endl;
    accesesd = false;
}

void reader(){

    while(true){
        cv2.wait(mLock,[]{return !accesesd;});
        if(Buffer.size() >= limit){
            cv1.wait(rLock, []{return itemsAvailable();});   
            read();
            cv1.notify_all();
        }
        cv2.notify_all();
    }
}

int main(){
    srand(time(0));
    thread th1(writer);
    thread th2(reader);
    th1.join();
    th2.join();
    return 0;
}