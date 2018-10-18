#include "QMyComboBox.h"
#include <qobject.h>
#include "LineModel/ParseLineInfo.h"

QMyComboBox::QMyComboBox()
{

}

QMyComboBox::~QMyComboBox()
{

}

void QMyComboBox::showPopup()
{
    clear();
    ParseLineInfo parse;
    int num = parse.getDispatchInfoByDB();
    if(num > 0)
    {
        for(int i = 0;i < num;i++)
        {
            addItem(QString::fromStdString(currDistpatchList[i].Dispatch_No));
        }
    }
    QComboBox::showPopup();
}

//void QMyComboBox::focusInEvent(QFocusEvent *event)
//{
//    emit getCurrPartNo();
//}
