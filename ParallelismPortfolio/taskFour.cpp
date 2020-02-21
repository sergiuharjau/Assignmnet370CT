#include "omp.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include "stdio.h"

std::vector<std::vector<int>> findClosestPairs(std::vector<std::vector<int>> inputVector)
{
    std::vector<std::vector<int>> vectorPairs = {};
    std::vector<int> blackList = {};

    #pragma omp parallel for schedule (static,1)
    for (int i=0; i < inputVector.size(); i++)
    {
        //looking at element i
        int distance = 100; //big number, we check newDistance < this
        int pair = -1; //no pair to start with

        bool inBlackList = false ;
        for(int elem: blackList)
        {
            //std::cout << "In blacklist: " << elem << std::endl;
            if (elem == i)
                inBlackList = true ;//if element already Paired, ignore
        }
        if (inBlackList)
                continue ;

        for (int j=i+1; j < inputVector.size(); j++)
        {
                //check all distances to pairs of j, find minimum
            bool inBlackList = false ;
            for(int elem: blackList)
            {
                if (elem == j)
                    inBlackList = true ;//if element already Paired, ignore
            }

            if (inBlackList)
                continue ;

            //std::cout << "Trying: " << i << " with " << j << std::endl;
            //measure distance from xyz of I to xyz of J 
            int newDistance = abs(inputVector[i][0]-inputVector[j][0]) + abs(inputVector[i][1]-inputVector[j][1]) + abs(inputVector[i][2]-inputVector[j][2]);
            //std::cout << "Distance: " << newDistance << std::endl;
            if (newDistance < distance)
            {
                distance = newDistance; //new minimum distance
                pair = j; //paired mate 
            }
        }

        std::vector<int> pairVector = {i, pair} ;
        vectorPairs.emplace_back(pairVector); //curent i plus the pair we like
        blackList.emplace_back(pair);
        blackList.emplace_back(i);
        //once we find the closest distance, add J particle to "blacklist" since it's already paired
    }
    return vectorPairs;
}

std::vector<int> findMidPoint(std::vector<int> particleA, std::vector<int> particleB)
{
//Finds midpoint of two given particles
    std::vector<int> midpoint = {0, 0, 0};

    for (int i = 0 ; i < 3 ; i ++)
    {
        midpoint[i] = std::min(particleA[i], particleB[i]) + (abs(particleA[i] - particleB[i]))/2;
        //std::cout << "Midpoint: " << midpoint[i] << " Particle: " << particleA[i] << " " << particleB[i] << std::endl; 
    }
    return midpoint ;
}

int main (void)
{
    std::vector<std::vector<int>> vectors= {{5,14,10}, {7,-8,-14}, {-2,9,8}, {15,-6,3}, {12,4,-5}, {4,20,17}, {-16,5,-1}, {-11,3,16}, {3,10,-10}, {-16,7,4}};
    std::vector<int> choice = {-1, 1};

    std::vector<std::vector<int>> pairs ;

    
    pairs = findClosestPairs(vectors);
    
    for (std::vector<int> pair : pairs)
    {
        std::cout << "Elements: " << pair[0] << " and "<< pair[1] << std::endl;
        for(int elem: vectors[pair[0]]) std::cout << elem << " " ;
        std::cout << " with ";
        for(int elem: vectors[pair[1]]) std::cout << elem << " " ;
        std::cout << std::endl;
        std::vector<int> midpoint = findMidPoint(vectors[pair[0]], vectors[pair[1]]);
        std::cout << "Midpoint: " << std::endl << midpoint[0] << "\t" << midpoint[1] << "\t" << midpoint[2] << std::endl ;
        std::cout << "Original Distance from A to midpoint: "  << abs(vectors[pair[0]][0] - midpoint[0]) + abs(vectors[pair[0]][1] - midpoint[1] ) + abs(vectors[pair[0]][2] - midpoint[2]) << std::endl;
        std::cout << "Original Distance from B to midpoint: " << abs(vectors[pair[1]][0] - midpoint[0]) + abs(vectors[pair[1]][1] - midpoint[1] ) + abs(vectors[pair[1]][2] - midpoint[2]) << std::endl << std::endl;
    }

    for (int loop=0; loop<10; loop++)
    {
        if (loop == 4 || loop == 9)
            std::cout << std::endl;
        for (int i=0; i < vectors.size(); i++)
        {
            if (loop == 0 || loop == 4 || loop == 9)
                std::cout << "Step: " << loop << " || Element: " << i << " || Values: " << vectors[i][0] << "\t" << vectors[i][1] << "\t" << vectors[i][2] << std::endl;
            
            int partner = -1 ;
            for (std::vector<int> pair : pairs)
            {
                if (pair[0] == i)
                    partner = pair[1];
                else if (pair[1] == i)
                    partner = pair[0];
            }
            
   
            //std::cout << "Pair: " << i << " " << partner << std::endl;
            std::vector<int> midpoint = findMidPoint(vectors[i], vectors[partner]); //THIS IS AN ERROR BECAUSE WE NEED TO SAVE THE MIDPOINT, NOT RECOMPUTE IT EVERY TIME
            //std::cout << "Midpoint: " << midpoint[0] << "\t" << midpoint[1] << "\t" << midpoint[2] << std::endl;
                        
                        //current element, random xyz +- 1 based on midpoint position
            int position = rand()%3; //random xyz
            vectors[i][position] += (midpoint[position] > vectors[i][position])?1:-1; 
            //std::cout << "Step: " << loop << " || Element: " << i << " || Values: " << vectors[i][0] << "\t" << vectors[i][1] << "\t" << vectors[i][2] << std::endl;
        }


    }


    return 0;
};