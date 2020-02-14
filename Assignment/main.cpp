#include <iostream>
#include <cmath>
#include <cstdlib>
#include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <iterator>
#include <chrono>
#include <mutex>   
#include <condition_variable>

// char dataBuffer[1];
// int index = 0 ; //position of buffer index

std::mutex mtx;
std::condition_variable cv;

// bool producerFinished = false ;
// bool consumerFinished = false ;
// bool content_exists = true ;

int sensorTurn = 0; 
int numberOfSensors = 6; 
bool sensorFinished = true;

int loops = 5;
int errorCode = 10;

void sensor(int currentSensor)
{
    for(int i= 0 ; i < loops ; i++)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while(sensorTurn != currentSensor ) cv.wait(lck);

        std::cout << "Sensor: " << currentSensor << std::endl;

        errorCode = rand() % 10 ;
        std::cout << "Error code: " << errorCode <<std::endl;
        switch(errorCode)
        {
            case 0: 
                std::cout << "Freewheeling" << std::endl;
                sensorTurn = -10; //puts the sensor on hold, signalling freewheeling
                break;
            case 1: 
                std::cout << "Sinking" << std::endl;
                sensorTurn = -11;
                break;
            case 2: 
                std::cout << "Blocked" << std::endl;
                sensorTurn = -12;
                break;
            default:
                std::cout << "No issues." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        sensorTurn += 1;  sensorTurn %= numberOfSensors; 
        cv.notify_all();
    }
}


int main (void){
    srand(time(NULL));
    std::thread sensor1(sensor, 0);
    std::thread sensor2(sensor, 1);
    std::thread sensor3(sensor, 2);
    std::thread sensor4(sensor, 3);
    std::thread sensor5(sensor, 4);
    std::thread sensor6(sensor, 5);

    // std::thread wheelDiagnosing();
    // std::thread earthConnection();

    // std::thread fixBlock();
    // std::thread fixFreewheel();
    // std::thread fixSink();

    sensor1.join();
    sensor2.join();
    sensor3.join();
    sensor4.join();
    sensor5.join();
    sensor6.join();


    return 0;
}
