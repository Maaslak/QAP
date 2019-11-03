#ifndef Problem_HEADER
#define Problem_HEADER

#include <cstdlib>
#include <cmath>
#include <fstream>

using namespace std;

enum ObjectiveType
{
	_Min = -1,
	_Max = 1
}; // Types of objectives

class Problem
{
public:
	// Type of the objective - min or max.
	ObjectiveType objectivetype;

	// Loads problem instance from the given file. Returns true if succesful.
	virtual bool load(char *filename){};
};

class QAP : public Problem
{
public:
	int n, **A, **B;

	bool load(char *filename);
};

#endif /* Problem_HEADER */
