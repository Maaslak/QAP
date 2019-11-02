#include "Solution.h"
#include "Problem.h"
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "You need to pass instance location/name as a prameter";
        return -1;
    }

    QAP qap;
    qap.load(argv[1]);
    Solution solution = Solution(qap);

    solution.initRandomSolution();
    solution.lessNaiveRandomSearch(10000);
    cout << "Random: " << solution.bestObjectiveValue << endl;

    solution.initRandomSolution();
    solution.greedyLocalSearch();
    cout << "Greedy: " << solution.bestObjectiveValue << endl;

    solution.initRandomSolution();
    solution.steepestLocalSearch();
    cout << "Steepest: " << solution.bestObjectiveValue << endl;
}