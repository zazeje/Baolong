#include "CraftParameter.h"

CraftParameter::CraftParameter()
{
    CraftCode = "";
    DeviationRange = 0;
    ErrorCode = "";
    JudgeMode = 0;
    MaxErrorNum = 0;
    MinPassRate = 0;
    NeedJudge = 0;
    StandardValue = 0;
    StandarMax = 0;
    StandarMin = 0;
    Vaule = "";
}

void CraftParameter::Clear()
{
    CraftCode.clear();
    DeviationRange = 0;
    ErrorCode.clear();
    JudgeMode = 0;
    MaxErrorNum = 0;
    MinPassRate = 0;
    NeedJudge = 0;
    StandardValue = 0;
    StandarMax = 0;
    StandarMin = 0;
    Vaule.clear();
}
