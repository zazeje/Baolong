#ifndef PARSECSV_H
#define PARSECSV_H

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <QObject>
#include <QTextStream>
#include <QFile>
#include "LineDevice/PlcDevice/PlcPointInfo.h"
#include "DataConvert/DataType.h"

using namespace std;

class ParseCsv
{

public:

    ParseCsv();

    vector<PlcPointInfo> ReadCsvFile(string filename);
    bool GetPlcVec(vector<PlcPointInfo> PlcInfo);
    bool WriteToCsvFile(vector<PlcPointInfo> info,string filename);

};





#endif // PARSECSV_H
