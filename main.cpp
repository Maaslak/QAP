#include "Solution.h"
#include "Problem.h"
#include <ctime>

using namespace std;

long runRandomAlgorithm(QAP, int);
long runGreedyAlgorithm(QAP);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "You need to pass instance location/name as a prameter";
        return -1;
    }

    QAP qap;
    qap.load(argv[1]);
    long result = runGreedyAlgorithm(qap);
    cout << "Result: " << result << endl;
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

long runGreedyAlgorithm(QAP qap){
    Solution solution = Solution(qap);
    solution.initRandomSolution();
    long currentMin = __LONG_MAX__;
    bool improved = true;
    while(improved){
        int tries = 0;
        while(tries < qap.n*qap.n){
            Solution neighbour = solution.getNextNeighbour();
            if(neighbour.objectiveValue < currentMin){
                currentMin = neighbour.objectiveValue;
                solution = neighbour;
                cout<<"New min: "<<currentMin<<endl;
                break;
            }
            tries++;  
        }
        improved = tries < qap.n*qap.n;
    }
    return currentMin;
} 