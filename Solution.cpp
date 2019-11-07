#include <unistd.h>
#include "Solution.h"
#include <vector>
#include <numeric>
#include <iostream>

void Solution::initRandomSolution()
{
	permutation = vector<int>(problem.n);
	bestObjectiveValue = numeric_limits<int>::max();
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
		objectiveFuncCallsNum++;
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
	objectiveFuncCallsNum++;
}

void Solution::naiveRandomSearch(int maxTime)
{
	clock_t begin = clock();
	initRandomSolution();
	calculateObjectiveValue();
	do
	{
		for (size_t i = 0; i < permutation.size() - 1; i++)
		{
			long second = i + rand() % (permutation.size() - i - 1);
			swap(permutation[i], permutation[second]);
		}
		calculateObjectiveValue();
		updateBestSolution();
	} while (double(clock() - begin) < maxTime);
}

void Solution::lessNaiveRandomSearch(int maxTime)
{
	clock_t begin = clock();
	do
	{
		for (size_t i = 0; i < permutation.size() - 1; i++)
		{

			long second = i + rand() % (permutation.size() - i - 1);
			updateObjectValue(i, second);
			swap(permutation[i], permutation[second]);
			updateBestSolution();
		}
	} while (double(clock() - begin) < maxTime);
}

void Solution::greedyLocalSearch()
{
	localSearch(_Greedy);
}

void Solution::steepestLocalSearch()
{
	localSearch(_Steepest);
}

void Solution::localSearch(LocalSearchAlgorithm algorithm)
{
	bool improved = true;
	while (improved)
	{
		improved = false;
		while (hasNextNeighbour())
		{
			checkNextNeighbour();
			if (objectiveValue < bestObjectiveValue)
			{
				updateBestSolution();
				improved = true;
				if (algorithm = _Greedy)
					break;
			}
		}
		nextSwap = make_tuple(1, 0);
	}
}

bool Solution::hasNextNeighbour()
{
	return get<0>(nextSwap) < problem.n;
}

void Solution::checkNextNeighbour()
{
	int swapA = get<0>(nextSwap);
	int swapB = get<1>(nextSwap);
	swap(permutation[swapA], permutation[swapB]);
	updateObjectValue(swapA, swapB);
	swapB++;
	if (swapB == swapA)
	{
		swapB = 0;
		swapA++;
	}
	nextSwap = make_tuple(swapA, swapB);
}

void Solution::heuristic(int maxTime)
{
	clock_t begin = clock();
	do
	{
		performHeuristic();
	} while (double(clock() - begin) < maxTime);
}

void Solution::performHeuristic(){
	int startingPlace = rand() % problem.n;
	for (int k = 0; k < problem.n; k++)
	{
		vector<bool> isPlaced = vector<bool>(problem.n);
		permutation[startingPlace] = k;
		isPlaced[k] = true;
		for (int i = 0; i < problem.n; i++)
		{ //places
			if(i == startingPlace) continue;
			int currentBest = -1;
			long currentMin = numeric_limits<long>::max();
			for (int j = 0; j < problem.n; j++)
			{ //objects
				if (isPlaced[j])
					continue;
				long value = problem.B[k][j] * problem.A[0][i] + problem.B[j][k] * problem.A[i][0];
				if (value < currentMin)
				{
					currentMin = value;
					currentBest = j;
				}
			}
			permutation[i] = currentBest;
			isPlaced[currentBest] = true;
		}
		calculateObjectiveValue();
		updateBestSolution();
	}
}

void Solution::setTime(double seconds)
{
	timeElapsed = seconds;
}

void Solution::setObjectiveFuncCallsNum(long calls_num)
{
	objectiveFuncCallsNum = calls_num;
}

long Solution::getObjectiveFuncCallsNum()
{
	return objectiveFuncCallsNum;
}

void Solution::save(string filename)
{
	ofstream file;
	file.open(filename);
	file << problem.n << " " << bestObjectiveValue << " " << timeElapsed << " " << objectiveFuncCallsNum << '\n';
	for (size_t i = 0; i < bestPermutation.size(); i++)
	{
		file << bestPermutation[i] << " ";
	}
	file << '\n';

	file.close();
}
