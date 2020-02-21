#include "omp.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "stdio.h"

int main (void)
{
    std::vector<std::vector<int>> vectors= {{5,14,10}, {7,-8,-14}, {-2,9,8}, {15,-6,3}, {12,4,-5}, {4,20,17}, {-16,5,-1}, {-11,3,16}, {3,10,-10}, {-16,7,4}};
    std::vector<int> choice = {-1, 1};

    #pragma omp parallel for schedule (static,1)
    for (int loop=0; loop<10; loop++)
    {
        if (loop == 4 || loop == 9)
            std::cout << std::endl;
        for (int i=0; i < vectors.size(); i++)
        {
            if (loop == 0 || loop == 4 || loop == 9)
                std::cout << "Step: " << loop << " || Element: " << i << " || Values: " << vectors[i][0] << "\t" << vectors[i][1] << "\t" << vectors[i][2] << std::endl;
            vectors[i][rand()%3] += choice[rand()%2]; 
                //current position, random xyz +- 1 
        }
    }
    return 0;
};