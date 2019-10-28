#include <unistd.h>
#include "Solution.h"
#include <vector>

void Solution::initRandomSolution(int len) {

	for (size_t i = 0; i < len; i++)
	{
		permutation.push_back(i);
	}

	for (size_t i = 0; i < len - 1; i++)
	{
		swap(permutation[i + rand() % (len - i - 1)], permutation[i]);
	}
	calculateObjectiveValue();
}

void Solution::calculateObjectiveValue(){
	long sum=0;
	for(size_t i=0; i<permutation.size(); i++){
		for(int j=0; j<permutation.at(i); j++){
			sum+=problem.A[i][j]*problem.B[i][j]+problem.A[j][i]*problem.B[j][i];
		}
	}
	objectiveValue=sum;
}

void Solution::greedyLocalSearch() {
	vector<int> test = vector<int>(0, 5);
}

void Solution::steepestLocalSearch() {
    sleep(1);
}
