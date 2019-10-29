#include <unistd.h>
#include "Solution.h"
#include <vector>

using namespace std;

void Solution::initRandomSolution() {
	int len = problem.n;

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
	for(size_t i=0; i<problem.n; i++){
		for(size_t j=0; j<problem.n; j++){
			int a = permutation.at(i);
			int b = permutation.at(j);
			sum += problem.B[a][b]*problem.A[i][j];
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

Solution Solution::getNextNeighbour(){
	Solution neighbour = Solution(problem);
	neighbour.permutation = permutation;
	int swapA = get<0>(nextSwap);
	int swapB = get<1>(nextSwap);
	swap(neighbour.permutation[swapA], neighbour.permutation[swapB]);
	neighbour.calculateObjectiveValue();
	swapB++;
	if(swapB == problem.n){
		swapB = 0;
		swapA++;
	}
	nextSwap = make_tuple(swapA, swapB);
	return neighbour;
}