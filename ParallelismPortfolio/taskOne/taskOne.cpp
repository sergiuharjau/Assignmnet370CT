#include "omp.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>  
#include <thread>
#include "stdio.h"

int cores = 0 ;

int main (void)
{
    #pragma omp parallel
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout <<"Found a core!" << std::endl;
        cores ++ ;
    };

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    std::cout << "Total number of cores: " << cores << std::endl ; 

    system("lscpu | grep 'CPU MHz'") ; 

    system("grep MemTotal /proc/meminfo") ;

    system("du -sh ~/Uni/Assignment370CT") ;

    return 0;
};