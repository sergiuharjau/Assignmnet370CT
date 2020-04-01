#include <iostream>
#include "mpi.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <string> 
#include "omp.h"
#include <fstream>
#include <math.h>

float get_mem_total() {
    std::string token;
    std::ifstream file("/proc/meminfo");
    while(file >> token) {
        if(token == "MemTotal:") {
            float mem;
            if(file >> mem) {
                return floor(mem*0.00001)/10 ;
            } else {
                return 0;       
            }
        }
        // ignore rest of the line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0; // nothing found
}

int get_speed_total() {
    std::string token;
    std::ifstream file("/proc/cpuinfo");
    while(file >> token) {
        if(token == "cpu") {
            file >> token;
            if (token == "MHz")
            {
              file >> token; 
              int mhz = 0 ;
              file >> mhz ; 
              return mhz;
            }
        }
        // ignore rest of the line
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return 0; // nothing found
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
  //int dest = 6;//atoi(argv[2]); // change to command line inputs again if you want to vary these
  int src = 0; //atoi(argv[1]);

  int cores = omp_get_num_procs();
  float memory = get_mem_total() ;
  int speed = get_speed_total() ; 

  std::cout << "\nHello, I am node: " << node_name << "\n I have " << cores << " processors. " << std::endl;
  std::cout << " My cpu speed: " << speed << " Mhz" << std::endl;
  std::cout << " My memory: " << memory << " GB" << std::endl;
  
          //Send them all to src, base node
  MPI_Send(&cores, 1, MPI_INT, src, 0, MPI_COMM_WORLD);
  MPI_Send(&speed, 1, MPI_INT, src, 1, MPI_COMM_WORLD);
  MPI_Send(&memory, 1, MPI_FLOAT, src, 2, MPI_COMM_WORLD);

  std::this_thread::sleep_for(std::chrono::milliseconds(700));

  if(rank == src){
    int totalCores = 0 ;
    int totalSpeed = 0 ;
    float totalMem = 0 ;

    float memory = 0;
    int received = 0;
    
    std::cout << "\n\n" << "IN HEAD NODE. " << node_name<< " \n " << std::endl;

    for(int i = 0; i < size ; i++){

        std::cout << std::endl;
	      MPI_Recv(&received, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Received: " << received << " cores; from node: " << i+2 << std::endl;
        totalCores +=received; 
        MPI_Recv(&received, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Received: " << received << " MhH; from node: " << i+2 << std::endl;
        totalSpeed +=received; 
        MPI_Recv(&memory, 1, MPI_FLOAT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Received: " << memory << " GB; from node: " << i+2 << std::endl;
        totalMem +=memory;
        
        received = 0 ; // to ensure we get fresh data
        memory = 0 ;
    }
    std::cout << "\n\nCluster Information: " << std::endl;
    std::cout << "Total Nodes: " << size << std::endl;
    std::cout << "Total Cores: " << totalCores << " cores" << std::endl;
    std::cout << "Total Speed: " << totalSpeed << " MHz" << std::endl;
    std::cout << "Total Memory: " << totalMem << " GB" << std::endl;
  }

  MPI_Finalize();
}

