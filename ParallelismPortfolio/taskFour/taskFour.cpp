#include "omp.h"
#include <iostream>
#include <vector>

std::vector<int> findMidPoint(std::vector<int> particleA, std::vector<int> particleB)
{
//Finds midpoint of two given particles
    std::vector<int> midpoint = {0, 0, 0};

    for (int i = 0 ; i < 3 ; i ++)
        midpoint[i] = std::min(particleA[i], particleB[i]) + (abs(particleA[i] - particleB[i]))/2;

    return midpoint ;
}

std::vector<std::vector<int>> findClosestPairs(std::vector<std::vector<int>> inputVector)
{
    std::vector<std::vector<int>> vectorPairs = {};
    std::vector<int> blackList = {};

    #pragma omp parallel for schedule (static,1)
    for (int i=0; i < inputVector.size(); i++)
    {
        int minDistance = 100; //big number, we check that newDistance < this
        int pair = -1; //no pair to start with

        for (int j=i+1; j < inputVector.size(); j++)
        {
            bool inBlackList = false ;
            for(int elem: blackList)
                if (elem == j || elem == i)
                    inBlackList = true ;//if element already Paired, ignore

            if (inBlackList) //if arg gets triggered, skip element
                continue ;

                        //measure distance from xyz of I to xyz of J 
            int newDistance = abs(inputVector[i][0]-inputVector[j][0]) + abs(inputVector[i][1]-inputVector[j][1]) + abs(inputVector[i][2]-inputVector[j][2]);
            
            if (newDistance < minDistance)
            {
                minDistance = newDistance; //new minimum distance
                pair = j; //paired particle
            }
        }

        if (pair == -1) //we didn't find a pair
            continue ;

        std::vector<int> midPoint = findMidPoint(inputVector[i], inputVector[pair]);

        std::vector<int> pairVector = {i, pair, midPoint[0],  midPoint[1], midPoint[2]} ;
        vectorPairs.emplace_back(pairVector); //curent i plus the pair we like

        blackList.emplace_back(pair);
        blackList.emplace_back(i);
          //once we find their midpoint, add paired particles to "blacklist" not to be tried again
    }
    return vectorPairs;
}

void printStateOfPairs(std::vector<std::vector<int>> pairs, std::vector<std::vector<int>> vectors)
{
    std::cout <<  std::endl << "STATE OF OUR PAIRED PARTICLES" << std::endl << std::endl ;
    for (std::vector<int> pair : pairs)
    {
        std::cout <<"Particles: " << pair[0] << " and " << pair[1] << std::endl << "{ ";
        for(int elem: vectors[pair[0]]) std::cout << elem << " " ;
        std::cout << "} and { ";
        for(int elem: vectors[pair[1]]) std::cout << elem << " " ;
        std::cout << "}" <<std::endl;

        std::vector<int> midpoint = {pair[2], pair[3], pair[4]} ;

        std::cout << "  Midpoint: " << std::endl << "  { " << midpoint[0] << " " << midpoint[1] << " " << midpoint[2] << " }"<<std::endl ;
        std::cout << "    Distance from particle " << pair[0] << " to midpoint: "  << abs(vectors[pair[0]][0] - midpoint[0]) + abs(vectors[pair[0]][1] - midpoint[1] ) + abs(vectors[pair[0]][2] - midpoint[2]) << std::endl;
        std::cout << "    Distance from particle " << pair[1] << " to midpoint: " << abs(vectors[pair[1]][0] - midpoint[0]) + abs(vectors[pair[1]][1] - midpoint[1] ) + abs(vectors[pair[1]][2] - midpoint[2]) << std::endl << std::endl;
    }
}

int main (void)
{
    srand(time(NULL));

    std::vector<std::vector<int>> vectors = {{5,14,10}, {7,-8,-14}, {-2,9,8}, {15,-6,3}, {12,4,-5}, {4,20,17}, {-16,5,-1}, {-11,3,16}, {3,10,-10}, {-16,7,4}};
    std::vector<std::vector<int>> pairs = findClosestPairs(vectors);
    std::vector<int> midpoint = {}; //need declaration here for scope availability when looping

    printStateOfPairs(pairs, vectors);
    
    for (int loop=0; loop<10; loop++)
    {
        if (loop==0 || loop == 4 || loop == 9)
            std::cout << "Step: " << loop << std::endl;
        
        #pragma omp parallel for schedule (static,1)
        for (int i=0; i < vectors.size(); i++)
        {
            if (loop == 0 || loop == 4 || loop == 9)
                std::cout << "  Element: " << i << " || Values: " << vectors[i][0] << "\t" << vectors[i][1] << "\t" << vectors[i][2] << std::endl;
            
            for (std::vector<int> pair : pairs)
                if (pair[0] == i || pair[1] == i) //finding relevant midpoint for current particle
                    midpoint = {pair[2], pair[3], pair[4]};

                        //current element, random xyz +- 1 based on midpoint position
            int position = rand()%3;
            vectors[i][position] += (midpoint[position] > vectors[i][position]) ? 1 : -1; 
        }
    }

    printStateOfPairs(pairs, vectors);

    return 0;
};