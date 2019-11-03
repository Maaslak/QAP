#include <fstream>
#include <iostream>
#include "Problem.h"

using namespace std;

bool QAP::load(char *instanceName)
{
    ifstream inFile;

    string filename = "data/";
    filename += instanceName + (string) ".dat";

    inFile.open(filename);

    if (!inFile)
    {
        cout << "File does not exists" << filename;
        return false;
    }

    inFile >> n;

    A = new int *[n];
    B = new int *[n];

    for (size_t i = 0; i < n; i++)
    {
        A[i] = new int[n];
        B[i] = new int[n];
    }

    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            if (!(inFile >> A[i][j]))
            {
                cout << "Broken file: " << filename;
                return false;
            }
        }
    }

    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            if (!(inFile >> B[i][j]))
            {
                cout << "Broken file: " << filename;
                return false;
            }
        }
    }
    return true;
}
