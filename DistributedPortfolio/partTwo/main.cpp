#include <iostream>
#include "mpi.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <string> 
#include "omp.h"
#include <vector>
#include <fstream>
#include <algorithm>

std::vector <std::string> readPoem()
{
  std::ifstream file("poem.txt");
  std::string str;
  std::vector <std::string> lines;
 
  while (std::getline(file, str))
  {
	  if(str.size() > 0)
	    lines.push_back(str);
  }

  return lines;
}

int main(int argc, char** argv) {

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  char node_name[MPI_MAX_PROCESSOR_NAME];
  int rank,size, namelen;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(node_name, &namelen);

  memset(node_name+namelen,0,MPI_MAX_PROCESSOR_NAME-namelen);
  int src = 0; //atoi(argv[1]);

  std::vector<std::string> wholePoem = readPoem();
  
    if (rank == src) //Head node sends out lines
    {

        std::cout << "\nThis is the head node. " << node_name << std::endl;

//        #pragma omp parallel for schedule (static,1) //If you comment this line, output is even nicer
        for (int i = 0; i < size; i ++)
        {
            int line_to_do = size -i -1;
            std::cout << "\nSending the line: " << wholePoem[line_to_do] << std::endl;
            std::cout << "To node: " << i << std::endl;
            MPI_Send(&line_to_do, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    //node 0 gets last poem line and so on
        }
    std::cout << "\nMoving on to jumbling.\n" << std::endl;
    }

  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  

  for (int i=0 ; i < size ; i++)
  {
    if(rank == i){   
        int poemLine = -1;
                //Head node sends out all the lines
        MPI_Recv(&poemLine, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        if (poemLine==-1) continue;

        std::cout << i <<". We are node: " << i << std::endl;
        std::string line = wholePoem[poemLine]; 
        std::cout  <<" "<< i <<". Original: " <<  line << std::endl;

        std::string word;
        std::vector<std::string> wordsVector ;

        for(auto individual : line){
            if (individual != ' ')
                word = word + individual ; 
            else
            {
                word = word + " ";
                wordsVector.emplace_back(word) ;
                word = "" ;
            }    
        }
         wordsVector.emplace_back(word) ;

        std::random_shuffle ( wordsVector.begin(), wordsVector.end() );

        std::cout << " " << i << ". Shuffled: " ;
        for (std::string word : wordsVector)
        {
            std::cout << word;
        }
        std::cout << std::endl;

    }
  }

  MPI_Finalize();
}

