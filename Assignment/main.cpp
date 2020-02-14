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

std::mutex mtx;
std::condition_variable cv;

int sensorTurn = 0; 
int numberOfSensors = 6; 

int loops = 1;
bool running = true;
int lastActivatedSensor = 0;

void sensor(int currentSensor)
{
    for(int i= 0 ; i < loops ; i++)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while(sensorTurn != currentSensor)
        {
            // std::cout << "Sensor that is waiting: " << currentSensor << std:: endl;
            // std::cout << "Current sensor turn: " << sensorTurn << std::endl;
            cv.wait(lck);
        }

        std::cout << "Sensor: " << currentSensor << std::endl;
        lastActivatedSensor = currentSensor;

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        sensorTurn += 1;  sensorTurn %= numberOfSensors; //to loop back from 6 to 0
        

        int sensorGenerator = rand() % 10 ;
        std::cout << "Error code: " << sensorGenerator <<std::endl;
        switch(sensorGenerator)
        {
            case 0: 
                sensorTurn = -10; 
                break; //puts the sensor on hold, signalling freewheeling to diagnsosis thread
            case 1: 
                sensorTurn = -11;
                break;
            case 2: 
                sensorTurn = -12;
                break;
            default:
                std::cout << "No issues." << std::endl;
        }

        cv.notify_all();
    }
}

void diagnosis()
{
    while(running)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (sensorTurn >= 0) cv.wait(lck) ;

        if (sensorTurn == -77) break; //end of program case

        std::cout << "In diagnosis! Issue:" << std::endl;

        switch(sensorTurn)
        {
            case -10:
                std::cout << "Freewheeling" << std::endl;
                break;
            case -11:
                std::cout << "Sinking" << std::endl;
                break;
            case -12:
                std::cout << "Blocked" << std::endl;   
                break; 
        }

        std::cout << "Fixing issue. Restarting sensors." << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        sensorTurn = (lastActivatedSensor+1) %numberOfSensors; //start from where we left off
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

    std::thread diagnosisThread(diagnosis);
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

    running = false; //kills remaining threads
    sensorTurn = -77; //turn_off state
    cv.notify_all(); //notifies remaining condition variables

    diagnosisThread.join();

    return 0;
}
