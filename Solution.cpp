#include <unistd.h>
#include "Solution.h"
#include <vector>
#include <numeric>

void Solution::initRandomSolution()
{
	permutation = vector<int>(problem.n);
	iota(permutation.begin(), permutation.end(), 0);

	for (size_t i = 0; i < problem.n - 1; i++)
	{
		swap(permutation[i + rand() % (problem.n - i - 1)], permutation[i]);
	}
	calculateObjectiveValue();
}

void Solution::updateBestSolution()
{
	if (objectiveValue < bestObjectiveValue)
	{
		bestObjectiveValue = objectiveValue;
		bestPermutation = permutation;
	}
}

void Solution::oneStepUpdate(int first, int second, int permFirst, int permSecond)
{
	permFirst = permutation[permFirst];
	permSecond = permutation[permSecond];
	objectiveValue += problem.A[first][second] * (problem.B[permFirst][permSecond] - problem.B[permutation[first]][permutation[second]]);
}

void Solution::updateObjectValue(int firstIdx, int secondIdx)
{
	if (firstIdx != secondIdx)
	{
		for (size_t i = 0; i < permutation.size(); i++)
		{
			if (i != firstIdx && i != secondIdx)
			{
				oneStepUpdate(i, firstIdx, i, secondIdx);
				oneStepUpdate(i, secondIdx, i, firstIdx);
				oneStepUpdate(firstIdx, i, secondIdx, i);
				oneStepUpdate(secondIdx, i, firstIdx, i);
			}
			if (i == firstIdx)
			{
				oneStepUpdate(i, secondIdx, secondIdx, firstIdx);
				oneStepUpdate(secondIdx, i, firstIdx, secondIdx);
			}
		}
	}
}

void Solution::calculateObjectiveValue()
{
	long sum = 0;
	for (size_t i = 0; i < permutation.size(); i++)
	{
		for (size_t j = 0; j < permutation.size(); j++)
		{
			int a = permutation.at(i);
			int b = permutation.at(j);
			sum += problem.B[a][b] * problem.A[i][j];
		}
	}
	objectiveValue = sum;
}

void Solution::naiveRandomSearch()
{
	initRandomSolution();
	calculateObjectiveValue();
}

void Solution::lessNaiveRandomSearch()
{
	for (size_t i = 0; i < permutation.size() - 1; i++)
	{

		long second = i + rand() % (permutation.size() - i - 1);
		updateObjectValue(i, second);
		swap(permutation[i], permutation[second]);
		updateBestSolution();
	}
}

void Solution::greedyLocalSearch()
{
	vector<int> test = vector<int>(0, 5);
}

void Solution::steepestLocalSearch()
{
	sleep(1);
}

bool Solution::hasNextNeighbour()
{
	return get<0>(nextSwap)<problem.n;
}

Solution Solution::getNextNeighbour()
{
	Solution neighbour = Solution(problem);
	neighbour.permutation = permutation;
	int swapA = get<0>(nextSwap);
	int swapB = get<1>(nextSwap);
	swap(neighbour.permutation[swapA], neighbour.permutation[swapB]);
	neighbour.calculateObjectiveValue();
	swapB++;
	if (swapB == problem.n)
	{
		swapB = 0;
		swapA++;
	}
	nextSwap = make_tuple(swapA, swapB);
	return neighbour;
}