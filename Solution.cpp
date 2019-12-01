#include <unistd.h>
#include "Solution.h"
#include <vector>
#include <numeric>
#include <iostream>


Solution::Solution(QAP* qap, int maxNumIter): problem(qap){
	metricsCollector = new Solution::MetricsCollector(problem->n, maxNumIter);
	permutation = new int[problem->n];
	bestPermutation = new int[problem->n];
}

Solution::~Solution(){
	delete(metricsCollector);
	delete(permutation);
	delete(bestPermutation);
}

void Solution::initRandomSolution()
{
	bestObjectiveValue = numeric_limits<int>::max();
	objectiveFuncCallsNum = 0;
	iota(permutation, permutation + problem->n, 0);

	for (size_t i = 0; i < problem->n - 1; i++)
	{
		swap(permutation[i + rand() % (problem->n - i - 1)], permutation[i]);
	}
	calculateObjectiveValue();
	metricsCollector->addStartObjectiveValue(objectiveValue);
}

void Solution::updateBestSolution()
{
	if (objectiveValue < bestObjectiveValue)
	{
		bestObjectiveValue = objectiveValue;
		copy(permutation, permutation + problem->n, bestPermutation);
	}
}

void Solution::oneStepUpdate(int &base, int first, int second, int permFirst, int permSecond)
{
	permFirst = permutation[permFirst];
	permSecond = permutation[permSecond];
	base += problem->A[first][second] * (problem->B[permFirst][permSecond] - problem->B[permutation[first]][permutation[second]]);
}

int Solution::calcObjectValueChange(int firstIdx, int secondIdx)
{
	int change = 0;
	if (firstIdx != secondIdx)
	{
		for (size_t i = 0; i < problem->n; i++)
		{
			if (i != firstIdx && i != secondIdx)
			{
				oneStepUpdate(change, i, firstIdx, i, secondIdx);
				oneStepUpdate(change, i, secondIdx, i, firstIdx);
				oneStepUpdate(change, firstIdx, i, secondIdx, i);
				oneStepUpdate(change, secondIdx, i, firstIdx, i);
			}
			if (i == firstIdx)
			{
				oneStepUpdate(change, i, secondIdx, secondIdx, firstIdx);
				oneStepUpdate(change, secondIdx, i, firstIdx, secondIdx);
			}
		}
		objectiveFuncCallsNum++;
	}
	return change;
}

void Solution::calculateObjectiveValue()
{
	long sum = 0;
	for (size_t i = 0; i < problem->n; i++)
	{
		for (size_t j = 0; j < problem->n; j++)
		{
			int a = permutation[i];
			int b = permutation[j];
			sum += problem->B[a][b] * problem->A[i][j];
		}
	}
	objectiveValue = sum;
	objectiveFuncCallsNum++;
}

void Solution::naiveRandomSearch(double maxTime)
{
	clock_t begin = clock();
	do
	{
		for (size_t i = 0; i < problem->n - 1; i++)
		{
			long second = i + rand() % (problem->n - i - 1);
			swap(permutation[i], permutation[second]);
		}
		calculateObjectiveValue();
		updateBestSolution();
	} while (double(clock() - begin) < maxTime);
}

void Solution::lessNaiveRandomSearch(double maxTime)
{
	clock_t begin = clock();
	do
	{
		for (size_t i = 0; i < problem->n - 1; i++)
		{

			long second = i + rand() % (problem->n - i - 1);
			objectiveValue += calcObjectValueChange(i, second);
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
		tuple<int, int> bestSwap = nextSwap;
		int currentBestChange = 0;
		while (hasNextNeighbour())
		{
			tuple<int, int> neighbourSwap = nextSwap;
			int change = checkNextNeighbour();
			if (change < currentBestChange)
			{
				bestSwap = neighbourSwap;
				currentBestChange = change;
				improved = true;
				if (algorithm == _Greedy){
					break;
				}
			}
		}
		if(improved){
			objectiveValue += calcObjectValueChange(get<0>(bestSwap), get<1>(bestSwap));
			swap(permutation[get<0>(bestSwap)], permutation[get<1>(bestSwap)]);
			updateBestSolution();
		}
		nextSwap = make_tuple(1, 0);
	}
}

void Solution::decrementTemerature(){
	this->asConfig.temperature = this->asConfig.alpha * this->asConfig.temperature;
}

void Solution::initTemperature(int numIter=100){
	double mean = 0;
	for (size_t i = 0; i < numIter; i++)
	{
		for (size_t j = 0; j < problem->n - 1; j++)
		{
			int prevObjVal = this->objectiveValue;
			long second = j + rand() % (problem->n - j - 1);
			int change = calcObjectValueChange(j, second);
			objectiveValue += change;
			mean += double(change) / (numIter * problem->n);
			swap(permutation[j], permutation[second]);
			updateBestSolution();
		}	
	}
	this->asConfig.temperature = -max(double(mean), 0.0) / log(0.95);
}

void Solution::simulatedAnnealing(NeighborhoodType neighborhoodType, double alpha){
	this->asConfig.alpha = alpha;
	if (neighborhoodType == NeighborhoodType::_2OPT)
		this->asConfig.L = this->problem->n * (this->problem->n - 1) / 2;
	if (neighborhoodType == NeighborhoodType::_3OPT)
		cout << "TODO";

	clock_t begin = clock();

	this->initTemperature();

	int l = 0;
	double threshold;
	int withoutImprovement = 0;
	do{
		l = (l + 1) % this->asConfig.L;
		if (l % this->asConfig.L == 0)
			this->decrementTemerature();

		if (!hasNextNeighbour())
			nextSwap = make_tuple(1, 0);
		
		tuple<int, int> neighbourSwap = nextSwap;
		
		int diffObjValue = checkNextNeighbour();

		threshold = exp(-diffObjValue / asConfig.temperature);

		if (diffObjValue < 0
			|| ((double) rand() / (RAND_MAX)) < threshold){
				objectiveValue += diffObjValue;
				swap(permutation[get<0>(neighbourSwap)], permutation[get<1>(neighbourSwap)]);
				updateBestSolution();
				if (diffObjValue < 0)
					withoutImprovement = 0;
		}
		if (diffObjValue >= 0)
			withoutImprovement++;
		
		
	}while(withoutImprovement < 10 * asConfig.L);
}

bool Solution::hasNextNeighbour()
{
	return get<0>(nextSwap) < problem->n;
}

int Solution::checkNextNeighbour()
{
	int swapA = get<0>(nextSwap);
	int swapB = get<1>(nextSwap);
	int result = calcObjectValueChange(swapA, swapB);
	swapB++;
	if (swapB == swapA)
	{
		swapB = 0;
		swapA++;
	}
	nextSwap = make_tuple(swapA, swapB);
	return result;
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
	int startingPlace = rand() % problem->n;
	for (int k = 0; k < problem->n; k++)
	{
		vector<bool> isPlaced = vector<bool>(problem->n);
		permutation[startingPlace] = k;
		isPlaced[k] = true;
		for (int i = 0; i < problem->n; i++)
		{ //places
			if(i == startingPlace) continue;
			int currentBest = -1;
			long currentMin = numeric_limits<long>::max();
			for (int j = 0; j < problem->n; j++)
			{ //objects
				if (isPlaced[j])
					continue;
				long value = problem->B[k][j] * problem->A[0][i] + problem->B[j][k] * problem->A[i][0];
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

void Solution::setTime(double clocks)
{
	timeElapsed = clocks;
}

double Solution::getTime(){
	return timeElapsed;
}

void Solution::setObjectiveFuncCallsNum(long calls_num)
{
	objectiveFuncCallsNum = calls_num;
}

long Solution::getObjectiveFuncCallsNum()
{
	return objectiveFuncCallsNum;
}

void Solution::addMetricToFile(long* tab, ofstream& file){
		for (size_t i = 0; i < metricsCollector->numMeas; i++)
		{
			file << tab[i] << " ";
		}
		file << '\n';
	};

void Solution::updateMetrics(){
	metricsCollector->updateMetrics(bestObjectiveValue, objectiveFuncCallsNum, bestPermutation, problem->n);
}

void Solution::save(string filename, int numIter)
{
	ofstream file;
	file.open(filename);
	file << problem->n << " " << timeElapsed / CLOCKS_PER_SEC / numIter << "\n";
	addMetricToFile(metricsCollector->startObjectiveValues, file);
	addMetricToFile(metricsCollector->objectiveValues, file);
	addMetricToFile(metricsCollector->numObjValCalls, file);
	for (size_t i = 0; i < metricsCollector->numMeas; i++)
	{
		for (size_t j = 0; j < problem->n; j++)
	{
		file << metricsCollector->permutations[i][j] << " ";
	}
	file << '\n';
	}

	file.close();
}


Solution::MetricsCollector::MetricsCollector(int n, int numMeas){
	it = 0;
	this->numMeas = numMeas;
	inst = new bool[numMeas];
	startObjectiveValues = new long[numMeas];
	objectiveValues = new long[numMeas];
	numObjValCalls = new long[numMeas];
	permutations = new long*[numMeas];

	for (size_t i = 0; i < numMeas; i++)
	{
		permutations[i] = new long[n];
	}
		
}

Solution::MetricsCollector::~MetricsCollector(){
	delete(inst);
	delete(startObjectiveValues);
	delete(objectiveValues);
	delete(numObjValCalls);
	for (size_t i = 0; i < this->numMeas; i++)
	{
		delete(permutations[i]);
	}
	delete(permutations);
}

void Solution::MetricsCollector::addStartObjectiveValue(long startObjectiveValue){
	startObjectiveValues[it] = startObjectiveValue;
}

void Solution::MetricsCollector::updateMetrics(long objectiveValue, long numObjValCall, int* permutation, int n){
	objectiveValues[it] = objectiveValue;
	numObjValCalls[it] = numObjValCall;
	copy(permutation, permutation + n, permutations[it]);
	it = (it + 1) % numMeas;
}
