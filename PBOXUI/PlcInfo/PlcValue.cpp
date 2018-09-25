#include "PlcValue.h"

PlcValue::PlcValue()
{
    registerType = "";
    startAddr = 0;
    length = 0;
    itemType = 0;
    values.clear();
}

void PlcValue::Clear()
{
    itemType = 0;
    registerType = "";
    startAddr = 0;
    length = 0;
    values.clear();
}
