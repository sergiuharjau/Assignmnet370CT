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

char dataBuffer[1];
int index = 0 ; //position of buffer index

std::mutex mtx;
std::condition_variable cv;

bool producerFinished = false ;
bool consumerFinished = false ;
bool content_exists = true ;

std::vector <std::string> readPoem()
{
  std::ifstream file("poem.txt");
  std::string str;
  std::vector <std::string> lines;
 
  while (std::getline(file, str))
  {
	  if(str.size() > 0)
	    lines.push_back(str + '\n');
  }

  return lines;
}

void producer(std::vector<std::string> content)
{
  for (std::string elem : content)
  {
    for (char character : elem)
    {
      std::unique_lock<std::mutex> lck(mtx);

      dataBuffer[index] = character ; //putting chars into buffer
      index ++ ;

      //std::cout << "PRODUCING: " << character << std::endl;

      if (index > 0)
      {
          index = 0 ; //start back at 0 

          producerFinished = true ; //buffer filled, let consumer read
          consumerFinished = false ;
          cv.notify_all() ; // notify consumer to start reading
        
          while (!consumerFinished) cv.wait(lck); // wait on Signal by consumer before we continue
      } 
    }
  }
  std::cout << "Finished producing poem." << std::endl;
  content_exists = false;
  producerFinished = true;
  cv.notify_all() ;
}

void consumer()
{
  std::string line = "";
  std::vector<std::string> fullContent;

  while (true)
  {
    std::unique_lock<std::mutex> lck(mtx);
    while (!producerFinished) cv.wait(lck);

    if (!content_exists) break;

    //std::cout << "CONSUMING BUFFER:" << std::endl;

    for (char elem : dataBuffer) //take every element of the dataBuffer
    {
      line += elem;

      if (elem == '\n')
      {
        fullContent.emplace_back(line); 
        line = "";
        std::cout << "\nLine finished. Poem so far:\n" << std::endl;
        for(std::string x:fullContent) std::cout << x;
        std::cout << std::endl;
      }
      else
        std::cout << line << std::endl;
    }

    //std::this_thread::sleep_for(std::chrono::milliseconds(500));

    consumerFinished = true ;
    producerFinished = false ;
    cv.notify_all(); //hands back control to producer
  }
  
  std::cout << "\nEntire poem has been transferred. Here it is:\n" << std:: endl;
  for(std::string x:fullContent) std::cout << x;
}


int main (void){

  std::vector<std::string> poem = readPoem();

  std::thread t_Consumer(producer, poem);
  std::thread t_Producer(consumer);

  t_Consumer.join();
  t_Producer.join();

  return 0;
}
