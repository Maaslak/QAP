#include "Solution.h"
#include "Problem.h"
#include <ctime>
#include <functional>
#include <algorithm>

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> algorithmNames{
        "heuristic",
        "greedyLocalSearch",
        "steepestLocalSearch",
        "lessNaiveRandomSearch"};
    vector<string>::iterator selectedAlgoritm = algorithmNames.end();

    if (argc == 3)
    {
        if ((selectedAlgoritm = find(algorithmNames.begin(), algorithmNames.end(), argv[2])) == algorithmNames.end())
        {
            cout << "Selected algorithm not in the list of available algorithms";
            return -1;
        }
    }
    else if (argc != 2)
    {
        cout << "You need to pass instance location/name as a prameter";
        return -1;
    }

    QAP qap;
    qap.load(argv[1]);
    Solution solution = Solution(qap);

    vector<function<void()>> algorithms{
        bind(&Solution::heuristic, ref(solution)),
        bind(&Solution::greedyLocalSearch, ref(solution)),
        bind(&Solution::steepestLocalSearch, ref(solution)),
        [&solution]() { return solution.lessNaiveRandomSearch(10000); }};

    if (selectedAlgoritm != algorithmNames.end())
    {
        solution.initRandomSolution();
        algorithms[selectedAlgoritm - algorithmNames.begin()]();
        cout << *selectedAlgoritm << ": " << solution.bestObjectiveValue << endl;
    }
    else
    {
        for (size_t i = 0; i < algorithms.size(); i++)
        {
            solution.initRandomSolution();
            algorithms[i]();
            cout << algorithmNames[i] << ": " << solution.bestObjectiveValue << endl;
        }
    }
}