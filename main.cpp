#include "Solution.h"
#include "Problem.h"
#include <ctime>

using namespace std;

long runRandomAlgorithm(QAP, int);
long runGreedyAlgorithm(QAP);
long runSteepestAlgorithm(QAP);
long runLocalSearchAlgorithm(QAP, bool);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "You need to pass instance location/name as a prameter";
        return -1;
    }

    QAP qap;
    qap.load(argv[1]);
    cout << "Random: " << runRandomAlgorithm(qap, 10000) << endl;
    cout << "Greedy: " << runGreedyAlgorithm(qap) << endl;
    cout << "Steepest: " << runSteepestAlgorithm(qap) << endl;
}

long runRandomAlgorithm(QAP qap, int maxTime)
{
    Solution solution = Solution(qap);
    long currentMin = __LONG_MAX__;
    clock_t begin = clock();
    do
    {
        solution.initRandomSolution();
        long value = solution.objectiveValue;
        if (value < currentMin)
        {
            currentMin = value;
        }
    } while (double(clock() - begin) < maxTime);
    return currentMin;
}

long runGreedyAlgorithm(QAP qap)
{
    return runLocalSearchAlgorithm(qap, true);
}

long runSteepestAlgorithm(QAP qap)
{
    return runLocalSearchAlgorithm(qap, false);
}

long runLocalSearchAlgorithm(QAP qap, bool isGreedy){
    Solution solution = Solution(qap);
    solution.initRandomSolution();
    long currentMin = __LONG_MAX__;
    bool improved = true;
    while(improved){
        improved = false;
        while(solution.hasNextNeighbour()){
            Solution neighbour = solution.getNextNeighbour();
            if(neighbour.objectiveValue < currentMin){
                currentMin = neighbour.objectiveValue;
                solution = neighbour;
                improved = true;
                if(isGreedy) break;
            }
        }
    }
    return currentMin;
} 