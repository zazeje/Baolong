#ifndef ARITHMETICHELPER_H
#define ARITHMETICHELPER_H

#include <iostream>
#include <string>
#include <stack>
#include <cstdlib>
#include <map>
#include "DataHelper/LogFile.h"

using namespace std;

class ArithmeticHelper
{

public:

    ArithmeticHelper();
    ~ArithmeticHelper();

    string UpdateFormula(string formula);
    double posfixCompute(double value,string formula);

private:
    string InfixToPostfix(string infix);

};
#endif //ARITHMETICHELPER_H
