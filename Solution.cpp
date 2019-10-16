#include <unistd.h>
#include "Solution.h"

void Solution::initRandomSolution(int len) {

	for (size_t i = 0; i < len; i++)
	{
		permutation.push_back(i);
	}

	for (size_t i = 0; i < len - 1; i++)
	{
		swap(permutation[i + rand() % (len - i - 1)], permutation[i]);
	}
}

void Solution::greedyLocalSearch() {
	sleep(1);
}

void Solution::steepestLocalSearch() {
    sleep(1);
}
