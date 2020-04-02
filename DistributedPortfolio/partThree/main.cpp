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

bool matchingIndex(std::string line, std::string received, int neededIndex)
{
    int actualIndex = 0;
    unsigned int characterCount = 0;

    std::string word = "";
    std::cout << "   Original line: " << line << std::endl;
    for(auto individual : line){
      characterCount ++;
        if (individual != ' ')
            word = word + individual ;
        if ((individual == ' ') || characterCount == line.size())
        {
            word = word + " "; //standard notation
            if (word==received)
            { 
              std::cout << "   Original Index: " << actualIndex << std::endl;
              if (actualIndex == neededIndex)
                return true;
            }
            word = "" ;
            actualIndex += 1;
        }    
    }
    return false; 
}

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
  srand(time(NULL));
  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  char node_name[MPI_MAX_PROCESSOR_NAME];
  int rank,size, namelen;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(node_name, &namelen);
  memset(node_name+namelen,0,MPI_MAX_PROCESSOR_NAME-namelen);
  int src = 0; 

  std::vector<std::string> wholePoem = readPoem();
  
    if (rank == src) //Head node sends out lines
    {
        std::cout << "\nThis is the head node. " << node_name << std::endl;

//If you comment this OMP line out, output is even nicer
        #pragma omp parallel for schedule (static,1) 
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

  std::this_thread::sleep_for(std::chrono::milliseconds(700)); //sync

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
        wordsVector.emplace_back(word + " ") ;

        std::random_shuffle ( wordsVector.begin(), wordsVector.end() );

        std::cout << " " << i << ". Shuffled: " ;
        for (std::string word : wordsVector)
            std::cout << word;
//
// START OF PART THREE WORK
        std::cout << std::endl << "  " << i << ". SENDING WORDS FOR CHECKING " << std::endl; 
        MPI_Send(&poemLine, 1, MPI_INT, src, 5, MPI_COMM_WORLD); //send poem line to head node

// SENDING FIRST WORD
        int choice = rand()%wordsVector.size();
        std::string toSend = wordsVector[choice]; //choose a word randomly
        int sizeString = toSend.size();

        MPI_Send(&sizeString, 1, MPI_INT, src, 0, MPI_COMM_WORLD);  //send string first
        MPI_Send(toSend.c_str(), toSend.size(), MPI_CHAR, src, 1, MPI_COMM_WORLD); //then the word itself                              
        MPI_Send(&choice, 1, MPI_INT, src, 2, MPI_COMM_WORLD);   // then the jumbled index    
        std::cout << "  " << i << ". FIRST WORD " << toSend <<" INDEX: " << choice << std::endl;   

// SENDING SECOND WORD
        choice = rand()%wordsVector.size();
        toSend = wordsVector[choice]; // now for the second word
        sizeString = toSend.size();

        MPI_Send(&sizeString, 1, MPI_INT, src, 3, MPI_COMM_WORLD);  //send string first
        MPI_Send(toSend.c_str(), toSend.size(), MPI_CHAR, src, 4, MPI_COMM_WORLD); //then the word itself                                                          
        MPI_Send(&choice, 1, MPI_INT, src, 5, MPI_COMM_WORLD);    // then the jumbled index      
        std::cout << "  " << i << ". SECOND WORD " << toSend <<" INDEX: " << choice << std::endl << std::endl;                  
      }
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(700)); //get stuff in sync

    if (rank == src)
    {
      std::cout << std::endl << std::endl << "ANALYSING RECEIVED DATA " << std::endl;
      for (int i=0; i < size ; i++)
      {
// SETUP
        int jumbledIndex;
        int stringSize; int poemLine;
        bool firstMatch = false;  bool secondMatch = false;

        std::cout << std::endl << std::endl << i << ". From node " << i << std::endl;

        MPI_Recv(&poemLine, 1, MPI_INT, i, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          //so we know which line we need to check the index on

// RETRIEVAL OF FIRST WORD 
        MPI_Recv(&stringSize, 1 , MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive word size 
        char *buf = new char[stringSize]; //allocate enough space into a char array
        MPI_Recv(buf, stringSize , MPI_CHAR, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive the word
        std::string received = buf; //make it a string
        received.resize(stringSize); //double check on resizing
        MPI_Recv(&jumbledIndex, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE); //receive the jumbled index

        std::cout << std::endl << " " << i <<".1 Word: [" << received << "]" << std::endl;
        std::cout << " " << i <<".1 Jumbled Index: " << jumbledIndex <<  std::endl;

        std::string line = wholePoem[poemLine];
        if(matchingIndex(line, received, jumbledIndex)) //check to see if the jumbled index matches the original
        {
            std::cout  << " Matching!" << std::endl;
            firstMatch = true;
        }
        else
          std::cout << " No match!" << std::endl;
        
// RETRIEVAL OF SECOND WORD
        MPI_Recv(&stringSize, 1 , MPI_INT, i, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        buf = new char[stringSize];
        MPI_Recv(buf, stringSize , MPI_CHAR, i, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        received = buf;
        received.resize(stringSize);
        MPI_Recv(&jumbledIndex, 1, MPI_INT, i, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::cout << std::endl <<" " << i <<".2 Word:  [" << received << "]" << std::endl;
        std::cout << " " << i <<".2 Jumbled Index: " << jumbledIndex <<  std::endl;

        if(matchingIndex(line, received, jumbledIndex))
        {
            std::cout  << " Matching!" << std::endl;
            secondMatch = true;
        }
        else
          std::cout << " No match!" << std::endl;

// FINAL ANALYSIS
        if(firstMatch && secondMatch)
          std::cout << std::endl << "RESULT: They both match! Nice one." << std::endl;
        else
          std::cout << std::endl << "RESULT: Not matching both. Sorry." << std::endl; 
      }
      std::cout << std::endl;
  }
  MPI_Finalize();
}

