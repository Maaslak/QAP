#ifndef Solution_HEADER
#define Solution_HEADER

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <tuple>
#include <limits>
#include "Problem.h"

using namespace std;

class Solution
{
private:
	void oneStepUpdate(int, int, int, int);
	void updateObjectValue(int, int);

public:
	// Problem associated with this solution
	QAP problem;

	// Value of objective function
	long objectiveValue;
	long bestObjectiveValue = numeric_limits<int>::max();

	// Solution holder
	vector<int> permutation;
	vector<int> bestPermutation;

	// Holder for next swap (to create a neighbour)
	tuple<int, int> nextSwap = make_tuple(0, 0);

	Solution() {}

	Solution(QAP qap) : problem(qap){};

	// Creates a copy of the given solution
	// Solution(Solution &solution)
	// {
	// 	objectiveValue = solution.objectiveValue;
	// 	permutation = solution.permutation;
	// }

	bool hasNextNeighbour();

	Solution getNextNeighbour();

	void initRandomSolution();

	void updateBestSolution();

	// Optimizes objective value by random algorithm with generating permutation every step
	void naiveRandomSearch();

	// Optimizes objective value by random algorithm with checking every swap in gen random solution
	void lessNaiveRandomSearch();

	// Optimizes locally the objective function by steepest algorithm.
	void steepestLocalSearch();

	// Optimizes locally the objective function by greedy algorithm.
	void greedyLocalSearch();

private:
	void calculateObjectiveValue();
};

#endif /* Solution_HEADER */
