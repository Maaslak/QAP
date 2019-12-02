#include <unistd.h>
#include "Solution.h"
#include <vector>
#include <numeric>
#include <iostream>
#include <algorithm>


Solution::Solution(QAP* qap, int maxNumIter): problem(qap){
	metricsCollector = new Solution::MetricsCollector(maxNumIter);
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

void Solution::tabuSearch()
{
	int k = problem->n/10;
	int L = 5*(problem->n*(problem->n-1));
	vector<TabuCandidate> candidates;
	vector<vector<int>> tabuList( problem->n , vector<int> (problem->n, 0));
	clock_t begin = clock();
	int withoutImprovement = 0;
	do
	{	
		candidates.clear();
		while (hasNextNeighbour())
		{
			tuple<int, int> swap = nextSwap;
			int change = checkNextNeighbour();
			if(tabuList[get<0>(swap)][get<1>(swap)]>0){
				tabuList[get<0>(swap)][get<1>(swap)]--;
				if(objectiveValue + change >= bestObjectiveValue) continue; //aspiration 
			}
			TabuCandidate candidate;
			candidate.swap = swap;
			candidate.change = change;
			candidates.push_back(candidate);
		}
		sort( candidates.begin( ), candidates.end( ), [ ]( const auto& a, const auto& b ){return a.change < b.change;});
		int size = min(k, (int)candidates.size());
		candidates.resize(size);

		int threshold = candidates[size-1].change;
		int i = 0;
		while(candidates[i].change<=threshold && i < candidates.size()){
			//Make step
			objectiveValue += candidates[i].change;
			swap(permutation[get<0>(candidates[i].swap)], permutation[get<1>(candidates[i].swap)]);
			if(objectiveValue < bestObjectiveValue){
				withoutImprovement = 0;
			}else{
				withoutImprovement++;
			}
			updateBestSolution();
			tabuList[get<0>(candidates[i].swap)][get<1>(candidates[i].swap)] = problem->n/4;
			i++;

			//Update changes for the rest
			for(int j=i; j<candidates.size(); j++)
			{
				candidates[j].change = calcObjectValueChange(get<0>(candidates[j].swap),get<1>(candidates[j].swap));
			}
			sort( candidates.begin( ) + i, candidates.end( ), [ ]( const auto& a, const auto& b ){return a.change < b.change;});
		}
		nextSwap = make_tuple(1, 0);
	} while (withoutImprovement < L);
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
	metricsCollector->updateMetrics(bestObjectiveValue, objectiveFuncCallsNum);
}

void Solution::save(string filename, int numIter)
{
	ofstream file;
	file.open(filename);
	file << problem->n << " " << timeElapsed / CLOCKS_PER_SEC / numIter << "\n";
	addMetricToFile(metricsCollector->startObjectiveValues, file);
	addMetricToFile(metricsCollector->objectiveValues, file);
	addMetricToFile(metricsCollector->numObjValCalls, file);
	for (size_t i = 0; i < problem->n; i++)
	{
		file << bestPermutation[i] << " ";
	}
	file << '\n';

	file.close();
}


Solution::MetricsCollector::MetricsCollector(int n){
	it = 0;
	numMeas = n;
	inst = new bool[n];
	startObjectiveValues = new long[n];
	objectiveValues = new long[n];
	numObjValCalls = new long[n];	
}

Solution::MetricsCollector::~MetricsCollector(){
	delete(inst);
	delete(startObjectiveValues);
	delete(objectiveValues);
	delete(numObjValCalls);
}

void Solution::MetricsCollector::addStartObjectiveValue(long startObjectiveValue){
	startObjectiveValues[it] = startObjectiveValue;
}

void Solution::MetricsCollector::updateMetrics(long objectiveValue, long numObjValCall){
	objectiveValues[it] = objectiveValue;
	numObjValCalls[it] = numObjValCall;
	it = (it + 1) % numMeas;
}
