#ifndef Solution_HEADER
#define Solution_HEADER

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include "Problem.h"

using namespace std;

class Solution
{
public:
	//Problem associated with this solution
	QAP problem;

	// Value of objective function
	long objectiveValue;

	// Solution holder
	vector<int> permutation;

	Solution() {}

	Solution(QAP qap) : problem(qap){};

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

private:
	void calculateObjectiveValue();
};

#endif /* Solution_HEADER */
