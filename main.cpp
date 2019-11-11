#include "Solution.h"
#include "Problem.h"
#include <ctime>
#include <functional>
#include <algorithm>
#include <sys/stat.h>
#include <sstream>
#include <string>

using namespace std;

int genExecId()
{
    string execIdFilename = "exec_id";
    ifstream file;
    int execId = 0;
    file.open(execIdFilename);
    if (file.good())
    {
        file >> execId;
        execId++;
    }
    file.close();
    ofstream outFile;
    outFile.open(execIdFilename);
    outFile << execId;
    outFile.close();
    return execId;
}

string resultFilename(char *instanceName, string algName, int execId)
{
    ostringstream result;
    result << "results/" << instanceName << "_" << algName << "_" << std::to_string(execId);
    return result.str();
}

long runAlg(Solution& solution, function<void()>* alg, long& maxTime){
    int numIter = 0;   
    clock_t begin = clock();
    solution.setObjectiveFuncCallsNum(0); 
    do
    {
        solution.initRandomSolution();
        (*alg)();
        numIter++;
        solution.updateMetrics();
        // cout << algorithmNames[i] << ": " << solution.bestObjectiveValue << endl;
    } while (numIter < 10 || double(clock() - begin) < 100);
    // Setting mean of elapsed time    
    solution.setTime(double(clock() - begin) / CLOCKS_PER_SEC);
    if (maxTime < solution.getTime()){
        maxTime = solution.getTime();
    }
    return numIter;
}

int main(int argc, char *argv[])
{
    vector<string> algorithmNames{
        "greedyLocalSearch",
        "steepestLocalSearch",
        "heuristic",
        "lessNaiveRandomSearch",
        "naiveRandomSearch"};
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

    int execId = genExecId();

    QAP qap;
    if (!qap.load(argv[1]))
        exit(1);
    Solution solution = Solution(&qap);

    function<void()> finiteAlgorithms[] = {
        bind(&Solution::greedyLocalSearch, ref(solution)),
        bind(&Solution::steepestLocalSearch, ref(solution)),
    };

    if (selectedAlgoritm != algorithmNames.end())
    {
        // solution.initRandomSolution();
        // algorithms[selectedAlgoritm - algorithmNames.begin()]();
        // solution.save(resultFilename(argv[1], *selectedAlgoritm, execId), );
        // cout << *selectedAlgoritm << ": " << solution.bestObjectiveValue << endl;
        cout << "TODO\n";
    }
    else
    {
        long maxTime = 0;
        size_t i = 0;
        for (function<void()> alg: finiteAlgorithms)
        {
            long numIter = runAlg(solution, &alg, maxTime);
            solution.save(resultFilename(argv[1], algorithmNames[i], execId), numIter);
            i++;
        }

        function<void()> infiniteAlgorithms[] = {
            bind(&Solution::heuristic, ref(solution), maxTime),
            bind(&Solution::lessNaiveRandomSearch, ref(solution), maxTime),
            bind(&Solution::naiveRandomSearch, ref(solution), maxTime),
        };
        
        for(function<void()> alg: infiniteAlgorithms){
            long numIter = runAlg(solution, &alg, maxTime);
            solution.save(resultFilename(argv[1], algorithmNames[i], execId), numIter);
            i++;
        }
    }
}