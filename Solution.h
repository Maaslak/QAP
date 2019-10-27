#ifndef Solution_HEADER
#define Solution_HEADER

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

class Solution
{
public:
	// Value of objective function
	double objectiveValue;

	// Solution holder
	vector<int> permutation;

	Solution() {}

	// Creates a copy of the given solution
	// Solution(Solution &solution)
	// {
	// 	objectiveValue = solution.objectiveValue;
	// 	permutation = solution.permutation;
	// }

	void initRandomSolution(int);

	// Optimizes locally the objective function by steepest algorithm.
	void steepestLocalSearch();

	// Optimizes locally the objective function by greedy algorithm.
	void greedyLocalSearch();
};

#endif /* Solution_HEADER */
