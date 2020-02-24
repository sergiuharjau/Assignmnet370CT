#include "omp.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>  
#include <thread>
#include "stdio.h"
#include <vector>

int cores = 0 ;

int main (void)
{
    std::vector<omp_sched_t> options = {omp_sched_static, omp_sched_dynamic, omp_sched_guided, omp_sched_auto};

    std::cout << "Scheduling type: " << std::endl << "1. Static" << std::endl << "2. Dynamic" << std::endl;
    std::cout << "3. Guided" << std::endl << "4. Auto " << std::endl << "Choice: " << std::endl;

    int choice;
    std::cin >> choice;

    omp_set_schedule(options[choice-1], 2); //indexed 0

    #pragma omp parallel for schedule(runtime)
    for (int loop=0; loop < omp_get_num_threads(); loop++)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout <<"In parallel section" << std::endl;
        cores ++ ;
    }

    return 0;
};