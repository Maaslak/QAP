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
    solution.initRandomSolution(qap.n);
    cout<<"Current objective value: "<<solution.objectiveValue<<endl;

    int num_iter = 0;
    clock_t begin = clock();

    do
    {
        solution.greedyLocalSearch();
        num_iter++;
    } while (double(clock() - begin) < 100 || num_iter < 10);

    cout << "Cool!\n";
}
