#include <iostream>
#include "mpi.h"
#include <cstring>
#include <chrono>
#include <thread>
#include <string> 
#include "omp.h"
#include <fstream>
#include <math.h>


int main(int argc, char** argv) {
  // Initialize the MPI environment
    srand(time(NULL));
    MPI_Init(NULL, NULL);
    char node_name[MPI_MAX_PROCESSOR_NAME];
    int rank,size, namelen;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(node_name, &namelen);
    memset(node_name+namelen,0,MPI_MAX_PROCESSOR_NAME-namelen);

    int current_node = 0;  //start on 0
    int previous_node = -1;
    int next_node = -1;

    if (rank == 0) //head node tells everyone how to start
        for(int i=0; i < size; i++)
        {
            MPI_Send(&current_node, 1, MPI_INT, i, 77, MPI_COMM_WORLD); //let other nodes know who's next
            MPI_Send(&previous_node, 1, MPI_INT, i, 88, MPI_COMM_WORLD);
        }

    int value = 3;

    for(int loop = 0; loop < 10; loop++)
    {
        std::cout << "\nLoop number: " << loop << std::endl;
        std::cout << " "<< rank << ". We are node " << rank << std::endl;

        std::cout << " "<< rank << ". Receiving metadata on node order" << std::endl;
        MPI_Recv(&previous_node, 1, MPI_INT, current_node, 88, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&current_node, 1, MPI_INT, current_node, 77, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << " "<< rank << ". Metadata received" << std::endl;

        if (rank == current_node) //only the node we want to run will run
        {
            std::cout << "  " << rank << ". Our turn to run! " << std::endl;
            while(next_node == current_node || next_node == previous_node || next_node==-1) 
                next_node = rand()%size; //pick a new random node, not current or previous though
                        
            if (previous_node != -1) // only runs after we send
            {
                MPI_Recv(&value, 1, MPI_INT, previous_node, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::cout  << "  "<< rank <<  ". Value received: " << value << std::endl;
                int multiplier = rand() % 3 + 1;
                std::cout << "  "<< rank <<  ". Before sending we are applying: x" << multiplier << std::endl;
                value = value * multiplier; //randomly *[1,2,3] on it
            }
            std::cout << "   "<< rank <<  ". Value we're sending: " << value << std::endl;
            std::cout << "   "<< rank <<  ". Next node to run: " << next_node << std::endl;
            MPI_Send(&value, 1, MPI_INT, next_node, 0, MPI_COMM_WORLD);

            for(int i=0; i < size; i++)
            {
                MPI_Send(&next_node, 1, MPI_INT, i, 77, MPI_COMM_WORLD); //let other nodes know who's next
                MPI_Send(&current_node, 1, MPI_INT, i, 88, MPI_COMM_WORLD); //let them know where we came from
            }
           std::cout << "  "<< rank << ". Sent metadata to all other nodes." << std::endl << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    MPI_Finalize();
}

