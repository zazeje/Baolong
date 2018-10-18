#ifndef CRAFTPARAMETER_H
#define CRAFTPARAMETER_H

#include <string>

using namespace std;

class CraftParameter
{
public:
    CraftParameter();

#if 1
    string CraftCode;
    string Vaule;
    int JudgeMode;
    double StandarMax;
    double StandarMin;
    double StandardValue;
    double DeviationRange;
    double MinPassRate;
    int MaxErrorNum;
    int NeedJudge;
    string ErrorCode;
    string PressFormulaName;
#endif

    //add by vincent 2017-4-13
    string testItemCode;
    string judgeEneble;
    string judgeMethod;
    string errorCode;
    string standardValue;
    string maxValue;
    string minValue;
    string deviation;
    string minPassrate;
    string maxLimit;

    void Clear();
};




#endif // CRAFTPARAMETER_H
