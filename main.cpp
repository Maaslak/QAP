#include "Solution.h"
#include <ctime>

using namespace std;

int main(){
    Solution solution = Solution();
    solution.initRandomSolution(10);

    int num_iter = 0;
    clock_t begin = clock();

    do
    {
        solution.greedyLocalSearch();
        num_iter++;
    } while (double(clock() - begin) < 100 || num_iter < 10);
    
    cout << "Cool!\n";
}
