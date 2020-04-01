#include <iostream>
#include "mpi.h"
#include <cstring>
int main(int argc, char** argv) {

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  char node_name[MPI_MAX_PROCESSOR_NAME];
int rank,size, namelen;
  int send_num = 5;
  int received = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Get_processor_name(node_name, &namelen);

  std::cout <<"Running on node: " << node_name << std::endl;

  memset(node_name+namelen,0,MPI_MAX_PROCESSOR_NAME-namelen);
  int dest = 6;//atoi(argv[2]); // change to command line inputs again if you want to vary these
  int src = 0; //atoi(argv[1]);

  //Everyone sends their details to src 
  MPI_Send(&send_num, 1, MPI_INT, src, 0, MPI_COMM_WORLD);

  
//  if (rank == src) {   
//    MPI_Send(&send_num, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
//    std::cout << "> In Sender node: " <<node_name<<"; Sent " << send_num << std::endl;
//    std::cout << "> In Sender node. Total Nodes: " << size << std::endl;
//  }
  if(rank == src){
     for(int i = 0; i < size ; i++){
	     MPI_Recv(&received, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
     	     std::cout << "In source node: " << received << std::endl;
		received = 0 ;
	}
     std::cout << "> In Receiver node: " << node_name << "; Got Number: " << received << std::endl;
  }


  MPI_Finalize();
}

