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
#define GPIO_PATH "/sys/class/gpio/"
using namespace std;


typedef int (*CallbackType)(int);

enum GPIO_DIRECTION { INPUT, OUTPUT };

enum GPIO_VALUE { LOW = 0, HIGH = 1 };

enum GPIO_EDGE { NONE, RISING, FALLING, BOTH };

class GPIO {
public:
    GPIO(int number);
    virtual int setDirection(GPIO_DIRECTION);
    virtual int setEdgeType(GPIO_EDGE);
    virtual int streamOpen();
    virtual int waitForEdge();
    virtual int streamWrite(GPIO_VALUE);
    virtual int streamClose();
    int write(string path, string filename, string value);
    int write(string path, string filename, int value);
    int number, debounceTime;
    string name, path;
    ofstream stream;
    int exportGPIO();
};

int main() {
    GPIO outGPIO(17), inGPIO(27);
    inGPIO.setDirection(INPUT);
    outGPIO.setDirection(OUTPUT);
    inGPIO.setEdgeType(RISING);
    outGPIO.streamOpen();
    outGPIO.streamWrite(LOW);
    cout << "Press the button:" << endl;
    inGPIO.waitForEdge();
    outGPIO.streamWrite(HIGH);
    outGPIO.streamClose();
    return 0;
}


int GPIO::setDirection(GPIO_DIRECTION dir) {
    switch (dir) {
    case INPUT: return this->write(this->path, "direction", "in");
        break;
    case OUTPUT: return this->write(this->path, "direction", "out");
        break;
    }
    return -1;
}
GPIO::GPIO(int number) {
	this->number = number;
	this->debounceTime = 0;
	//this->togglePeriod=100;
	//this->toggleNumber=-1; //infinite number
	//this->callbackFunction = NULL;
	//this->threadRunning = false;

	ostringstream s;
	s << "gpio" << number;
	this->name = string(s.str());
	this->path = GPIO_PATH + this->name + "/";
	this->exportGPIO();
	// need to give Linux time to set up the sysfs structure
	usleep(250000); // 250ms delay
}

int GPIO::setEdgeType(GPIO_EDGE value) {
    switch (value) {
    case NONE: return this->write(this->path, "edge", "none");
        break;
    case RISING: return this->write(this->path, "edge", "rising");
        break;
    case FALLING: return this->write(this->path, "edge", "falling");
        break;
    case BOTH: return this->write(this->path, "edge", "both");
        break;
    }
    return -1;
}

int GPIO::streamOpen() {
    stream.open((path + "value").c_str());
    return 0;
}

int GPIO::streamWrite(GPIO_VALUE value) {
    stream << value << std::flush;
    return 0;
}

int GPIO::streamClose() {
    stream.close();
    return 0;
}

int GPIO::waitForEdge() {
    this->setDirection(INPUT); // must be an input pin to poll its value
    int fd, i, epollfd, count = 0;
    struct epoll_event ev;
    epollfd = epoll_create(1);
    if (epollfd == -1) {
        perror("GPIO: Failed to create epollfd");
        return -1;
    }
    if ((fd = open((this->path + "value").c_str(), O_RDONLY | O_NONBLOCK)) == -1) {
        perror("GPIO: Failed to open file");
        return -1;
    }

    //ev.events = read operation | edge triggered | urgent data
    ev.events = EPOLLIN | EPOLLET | EPOLLPRI;
    ev.data.fd = fd; // attach the file file descriptor

    //Register the file descriptor on the epoll instance, see: man epoll_ctl
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
        perror("GPIO: Failed to add control interface");
        return -1;
    }
    while (count <= 1) {
        // ignore the first trigger
        i = epoll_wait(epollfd, &ev, 1, -1);
        if (i == -1) {
            perror("GPIO: Poll Wait fail");
            count = 5; // terminate loop
        }
        else {
            count++; // count the triggers up
        }
    }
    close(fd);
    if (count == 5) return -1;
    return 0;
}
int GPIO::write(string path, string filename, string value){
   ofstream fs;
   fs.open((path + filename).c_str());
   if (!fs.is_open()){
	   perror("GPIO: write failed to open file ");
	   return -1;
   }
   fs << value;
   fs.close();
   return 0;
}
int GPIO::exportGPIO(){
   return this->write(GPIO_PATH, "export", this->number);
}
int GPIO::write(string path, string filename, int value){
   stringstream s;
   s << value;
   return this->write(path,filename,s.str());
}