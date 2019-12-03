#include "Solution.h"
#include "Problem.h"
#include <ctime>
#include <functional>
#include <algorithm>
#include <sys/stat.h>
#include <sstream>
#include <string>
#include "boost/program_options.hpp"

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

string resultFilename(string instanceName, string algName, int execId, int numIter)
{
    ostringstream result;
    result << "results/" << instanceName << "_" << algName << "_" << std::to_string(execId) << "_" << std::to_string(numIter);
    return result.str();
}

long runAlg(Solution& solution, function<void()>* alg, int maxNumIter){
    int numIter = 0;   
    clock_t begin = clock();
    solution.setObjectiveFuncCallsNum(0); 
    do
    {
        solution.initRandomSolution();
        (*alg)();
        numIter++;
        if (numIter < maxNumIter + 1)
            solution.updateMetrics();
        // cout << algorithmNames[i] << ": " << solution.bestObjectiveValue << endl;
    } while (numIter < maxNumIter + 1 || double(clock() - begin) < 100);
    // Setting mean of elapsed time    
    solution.setTime(double(clock() - begin));

    return numIter;
}

long runAlgMaxTime(Solution& solution, function<void()>* alg, int maxNumIter, double& maxTime){
    long numIter = runAlg(solution, alg, maxNumIter);
    if (maxTime < solution.getTime()){
        maxTime = solution.getTime();
    }
    return numIter;
}

bool process_command_line(int argc, char** argv, string& instanceName, string& selectedAlgorithm, int& maxNumIter, double& maxTime){
    try
    {
        namespace po = boost::program_options;
        po::options_description desc("Options");

        desc.add_options()
            ("help", "produce help message")
            ("instance_name", po::value<string>(&instanceName)->required(), "instance name available in data path")
            ("selected_algorithm", po::value<string>(&selectedAlgorithm), "specify to select algorithm")
            ("num_iter", po::value<int>(&maxNumIter)->default_value(10), "number of iterations")
            ("max_time", po::value<double>(&maxTime), "max of execution time required to infinite algorithms")
        ;
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
    
        if (vm.count("help")) {
            cout << desc << '\n';
            return false;
        }
        po::notify(vm);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    catch(...){
        std::cerr << "Unknown error!\n";
        return false;
    }

    return true;    
}

int main(int argc, char *argv[])
{
    srand (time(NULL));
    string instanceName, selectedAlgoritm;
    int maxNumIter;
    double maxTime = 0;

    if (!process_command_line(argc, argv, instanceName, selectedAlgoritm, maxNumIter, maxTime))
        return -1;

    vector<string> algorithmNames{
        "greedyLocalSearch",
        "steepestLocalSearch",
        "tabuSearch",
        "simulatedAnnealing",
        "lessNaiveRandomSearch",
        "naiveRandomSearch",
        "heuristic"
        };

    int execId = genExecId();

    QAP qap;
    if (!qap.load(instanceName))
        exit(1);
    Solution solution = Solution(&qap, maxNumIter);

    int finiteAlgorithmsSize = 4;

    function<void()> finiteAlgorithms[finiteAlgorithmsSize] = {
        bind(&Solution::greedyLocalSearch, ref(solution)),
        bind(&Solution::steepestLocalSearch, ref(solution)),
        bind(&Solution::tabuSearch, ref(solution)),
        bind(&Solution::simulatedAnnealing, ref(solution), NeighborhoodType::_2OPT, 0.9)
    };

    if (selectedAlgoritm.length())
    {
        vector<string>::iterator it;
        if ((it = std::find(algorithmNames.begin(), algorithmNames.end(), selectedAlgoritm)) != algorithmNames.end())
        {
            int alg_idx = it - algorithmNames.begin();
            long numIter;
            if (alg_idx < finiteAlgorithmsSize){
                numIter = runAlg(solution, &finiteAlgorithms[alg_idx], maxNumIter);
            }
            else
            {
                if (maxTime <= 0){
                    cout << "Selected infinite algorithm requires maxTime to be specified";
                    exit(-1);
                }
                function<void()> infiniteAlgorithms[] = {
                    bind(&Solution::lessNaiveRandomSearch, ref(solution), maxTime),
                    bind(&Solution::naiveRandomSearch, ref(solution), maxTime),
                    bind(&Solution::heuristic, ref(solution), maxTime),
                };
                numIter = runAlg(solution, &infiniteAlgorithms[alg_idx], maxNumIter);   
            }
            solution.save(resultFilename(instanceName, algorithmNames[alg_idx], execId, maxNumIter), numIter);
            
        }
        else{
            cout << "Selected algorithm not in the list of available algorithms";
            exit(-1);
        }
    }
    else
    {
        maxTime = 0;
        size_t i = 0;
        for (function<void()> alg: finiteAlgorithms)
        {
            long numIter = runAlgMaxTime(solution, &alg, maxNumIter, maxTime);
            solution.save(resultFilename(instanceName, algorithmNames[i], execId, maxNumIter), numIter);
            i++;
        }

        maxTime = maxTime/10;

        function<void()> infiniteAlgorithms[] = {
            bind(&Solution::lessNaiveRandomSearch, ref(solution), maxTime),
            bind(&Solution::naiveRandomSearch, ref(solution), maxTime),
            bind(&Solution::heuristic, ref(solution), maxTime)
        };
        
        for(function<void()> alg: infiniteAlgorithms){
            long numIter = runAlg(solution, &alg, maxNumIter);
            solution.save(resultFilename(instanceName, algorithmNames[i], execId, maxNumIter), numIter);
            i++;
        }
    }
}