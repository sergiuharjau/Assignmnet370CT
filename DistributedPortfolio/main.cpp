#include <iostream>
#include "mpi.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <string> 
#include "omp.h"

int main(int argc, char** argv) {

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  char node_name[MPI_MAX_PROCESSOR_NAME];
  int rank,size, namelen;
  int received = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(node_name, &namelen);

  //std::cout <<"Running on node: " << node_name << std::endl;

  memset(node_name+namelen,0,MPI_MAX_PROCESSOR_NAME-namelen);
  //int dest = 6;//atoi(argv[2]); // change to command line inputs again if you want to vary these
  int src = 0; //atoi(argv[1]);

  //Everyone sends their details to src 

  // std::string node = node_name ;

  // std::string str = "echo 'Node: " + node + "' >> cpu_info.txt && lscpu | grep 'CPU MHz' >> cpu_info.txt" ;
  // //str = str + "echo 'n\n' >> cpu_info.txt";

  // const char *command = str.c_str(); 
  // system(command) ; 

  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  int cores = omp_get_num_procs();

  std::cout << "Hello, I am node: " << node_name << "\n I have " << cores << " processors. " << std::endl;
  system("lscpu | grep 'CPU MHz'") ;
  system("grep MemTotal /proc/meminfo") ;

          //Send them all to src, base node
  MPI_Send(&cores, 1, MPI_INT, src, 0, MPI_COMM_WORLD);

  if(rank == src){
    int total = 0 ;
    for(int i = 0; i < size ; i++){
	      MPI_Recv(&received, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
     	  std::cout << "In source node received: " << received << " cores; from node: " << i+2 << std::endl;
        total +=received; 
        received = 0 ; // to ensure we get fresh data
    }
    std::cout << "\nTotal cores: " << total << std::endl;
    std::cout << "\nTotal Nodes: " << size << std::endl;
    std::cout << "\nNode information: " << std::endl;
    system("cat cpu_info.txt");
    system("rm cpu_info.txt");
  }

  MPI_Finalize();
}

