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
