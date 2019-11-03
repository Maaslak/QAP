#ifndef Solution_HEADER
#define Solution_HEADER

#include <cstdlib>
#include <cmath>
#include <fstream>
#include <vector>
#include <iostream>
#include <tuple>
#include <limits>
#include <list>
#include "Problem.h"

using namespace std;

enum LocalSearchAlgorithm
{
	_Greedy = 0,
	_Steepest = 1
};

class Solution
{
private:
	void oneStepUpdate(int, int, int, int);
	void updateObjectValue(int, int);

	// time elapsed (needs to be set outside) with setTime
	double timeElapsed;

	long objectiveFuncCallsNum = 0;

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
	tuple<int, int> nextSwap = make_tuple(1, 0);

	Solution() {}

	Solution(QAP qap) : problem(qap){};

	// Creates a copy of the given solution
	// Solution(Solution &solution)
	// {
	// 	objectiveValue = solution.objectiveValue;
	// 	permutation = solution.permutation;
	// }

	bool hasNextNeighbour();

	void checkNextNeighbour();

	void initRandomSolution();

	void updateBestSolution();

	// Optimizes objective value by random algorithm with generating permutation every step
	void naiveRandomSearch(int);

	// Optimizes objective value by random algorithm with checking every swap in gen random solution
	void lessNaiveRandomSearch(int);

	// Optimizes locally the objective function by steepest algorithm.
	void steepestLocalSearch();

	// Optimizes locally the objective function by greedy algorithm.
	void greedyLocalSearch();

	// Optimizes objective value by using simple heuristic
	void heuristic();

	void setTime(double);

	void setObjectiveFuncCallsNum(long);

	long getObjectiveFuncCallsNum();

	void save(string);

private:
	void calculateObjectiveValue();

	void localSearch(LocalSearchAlgorithm);
};

#endif /* Solution_HEADER */
