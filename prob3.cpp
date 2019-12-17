#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<cstdlib>
#include<cstdio>
#include<fcntl.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<pthread.h>
#include <wiringPi.h>
#define GPIO_PATH "/sys/class/gpio/"
#define BUTTON_ONE 17
#define BUTTON_TWO 27
using namespace std;

int totalCount=0;
void callBackAction(void){
    cout<<"Button Pressed."<<endl;
    totalCount+=1;
    if(totalCount==10){
        cout<<"Terminated"<<endl;
        exit();
    }
        
}

int main() {
    wiringPiSetupGpio();
    pinMode(BUTTON_ONE,INPUT);
    pinMode(BUTTON_TWO,INPUT);
    
    cout << "Press the button:" << endl;
    wiringPiISR(BUTTON_ONE, INT_EDGE_RISING, &callBackAction);
    wiringPiISR(BUTTON_TWO, INT_EDGE_RISING, &callBackAction);
    while(totalCount<10){
        usleep(5000000);
        cout<<totalCount<<endl;
    }
    
    return 0;
}