#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <mutex>   
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

int sensorTurn = 0; 
int lastActivatedSensor = 0;

int loops = 2;
bool turnOff = false;

void sensor(int currentSensor)
{
    for(int i= 0 ; i < loops ; i++)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while(sensorTurn != currentSensor)
            cv.wait(lck);

        std::this_thread::sleep_for(std::chrono::milliseconds(500)); //for nicer terminal display
        std::cout << "Sensor: " << currentSensor << std::endl;
        
        lastActivatedSensor = currentSensor;
        sensorTurn += 1;  sensorTurn %= 6; //to loop back from 6 to 0

        int errorGenerator = rand() % 10; //30% of an issue on every sensor
        switch(errorGenerator)
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
    while(true)
    {
        std::unique_lock<std::mutex> lck(mtx); //while in any other threads, sleep
        while (!((sensorTurn < 0 && sensorTurn > -20) || turnOff))  cv.wait(lck) ;
        if (turnOff) break; //end of program case

        std::cout << "In diagnosis! Issue: ";
        switch(sensorTurn)
        {
            case -10:
                std::cout << "Freewheeling" << std::endl;
                sensorTurn = -30;
                break;
            case -11:
                std::cout << "Blocked" << std::endl;
                sensorTurn = -31;
                break;
            case -12:
                std::cout << "Sinking" << std::endl; 
                sensorTurn = -32;
                break; 
        }
        cv.notify_all();
    }
}

void fixFreewWeel()
{
    while (true)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (!(sensorTurn == -30 || turnOff))  cv.wait(lck);
        if (turnOff) break; //end of program case

        std::cout << "Trying to fix free wheeling. " << std::endl;
        int fixing = rand()%3 ; //30% chance of fixing ourselves
        if (fixing == 0)
        {
            std::cout<< "Onboard Fix Worked. Restarting sensors." << std::endl;
            sensorTurn = (lastActivatedSensor+1) % 6; //restart sensor, we've fixed the issue
        }
        else
        {
            std::cout<< "Asking earth for answers." << std::endl;
            sensorTurn = -55; //hand control back to earth
        }
        cv.notify_all();
    }
}

void fixBlockedWheel()
{
    while (true)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (!(sensorTurn == -31 || turnOff))  cv.wait(lck);
        if (turnOff) break; //end of program case

        std::cout << "Trying to fix blocked wheel. " << std::endl;
        int fixing = rand()%4 ; //50% chance of fixing ourselves
        if (fixing < 2)
        {
            std::cout<< "Onboard Fix Worked. Restarting sensors." << std::endl;
            sensorTurn = (lastActivatedSensor+1) % 6; //restart sensor, we've fixed the issue
        }
        else
        {
            std::cout<< "Asking earth for answers." << std::endl;
            sensorTurn = -55; //hand control back to earth
        } 
        cv.notify_all();
    }
}

void fixSinkingWheel()
{
    while (true)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (!(sensorTurn == -32 || turnOff))  cv.wait(lck);
        if (turnOff) break; //end of program case

        std::cout << "Trying to fix sinking wheel. " <<std::endl;
        int fixing = rand()%10 ; //10% chance of fixing ourselves
        if (fixing == 0)
        {
            std::cout<< "Onboard Fix Worked. Restarting sensors." << std::endl;
            sensorTurn = (lastActivatedSensor+1) % 6; //restart sensor, we've fixed the issue
        }
        else
        {
            std::cout<< "Asking earth for answers." << std::endl;
            sensorTurn = -55; //hand control back to earth
        }
        cv.notify_all();
    }
}

void earthConnection()
{
    while (true)
    {
        std::unique_lock<std::mutex> lck(mtx);
        while (!(sensorTurn == -55 || turnOff))  cv.wait(lck);
        if (turnOff) break; //end of program case

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        std::cout << "Earth saves the day! Restarting sensors." << std::endl;
        sensorTurn = (lastActivatedSensor+1) % 6; //restart sensor, we've fixed the issue

        cv.notify_all();
    }
}

int main (void){
    srand(time(NULL)); //provides truly random numbers

    std::thread diagnosisThread(diagnosis);
    std::thread earthConnectionThread(earthConnection);

    std::thread fixFreewheelThread(fixFreewWeel);
    std::thread fixBlockedWheelThread(fixBlockedWheel);
    std::thread fixSinkingWheelThread(fixSinkingWheel);

    std::thread sensor1(sensor, 0);
    std::thread sensor2(sensor, 1);
    std::thread sensor3(sensor, 2);
    std::thread sensor4(sensor, 3);
    std::thread sensor5(sensor, 4);
    std::thread sensor6(sensor, 5); //sensors stop based on loop variable

    sensor1.join(); //prevents thread from running indefinitely
    sensor2.join(); 
    sensor3.join(); 
    sensor4.join(); 
    sensor5.join(); 
    sensor6.join();

    turnOff = true; //kills remaining threads
    cv.notify_all(); //notifies remaining condition variables

    diagnosisThread.join();
    fixFreewheelThread.join();
    fixBlockedWheelThread.join();
    fixSinkingWheelThread.join();
    earthConnectionThread.join();

    return 0;
}