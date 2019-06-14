#-------------------------------------------------
#
# Project created by QtCreator 2016-05-17T17:01:52
#
#-------------------------------------------------

QT       += core gui sql xml 

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PBOX
TEMPLATE = app
target.path = /home/root/App
INSTALLS += target
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += /opt/fsl-imx-fb/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/include/mysql
INCLUDEPATH += /opt/fsl-imx-fb/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/include/libxml2
INCLUDEPATH += /opt/fsl-imx-fb/3.14.52-1.1.1/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/usr/include

LIBS +=  -lmysql -lxml2 -lz -ljsoncpp -liconv -llog4c


SOURCES +=\
    LineDriver/DeviceDriver.cpp \
    LineDevice/Device/ComDevice.cpp \
    LineDevice/Device/DeviceThread.cpp \
    LineDevice/Device/SerialPortParameters.cpp \
    LineDevice/Device/TcpDevice.cpp \
    LineDevice/Device/UdpDevice.cpp \
    LineDevice/DeviceThreads/ThreadLaserMarker.cpp \
    LineDevice/MarkerDevice/LaserMarker.cpp \
    LineDevice/PlcDevice/FxPlc.cpp \
    LineDevice/ProgramDevice/CycloneProgram.cpp \
    LineDevice/ProgramDevice/CycloneProUdp.cpp \
    DataHelper/MemoryDBHelper.cpp \
    DataConvert/DataType.cpp \
    LineDevice/DeviceThreads/ThreadFxPlc.cpp \
    LineModel/ConfigLoader.cpp \
    LineDevice/Device/ModBus.cpp \
    LineDevice/PlcDevice/DagongPlc.cpp \
    LineDevice/DeviceThreads/ThreadDgPlc.cpp \
    LineDevice/DeviceThreads/ThreadCyclone.cpp \
    LineDevice/MultimeterDevice/AgilentMultimeterTcp.cpp \
    LineDevice/MultimeterDevice/AgilentMultimeterSerial.cpp \
    DataHelper/MySQLHelper.cpp \
    DataCommunication/TcpServer.cpp \
    LineModel/DeviceConfig.cpp \
    LineDevice/DeviceThreads/ThreadDaMaSR.cpp \
    LineDevice/DeviceThreads/ThreadKeyenceSR.cpp \
    LineDevice/DeviceThreads/ThreadWenglorSR.cpp \
    LineDevice/ScannerDevice/DataManSR.cpp \
    LineDevice/ScannerDevice/KeyenceSR.cpp \
    LineDevice/ScannerDevice/WenglorSR.cpp \
    LineDriver/LogicalDriver.cpp \
    LineDevice/DeviceThreads/ThreadAgilent.cpp \
    LineDevice/TpmsHandleDevice/TpmsHandle.cpp \
    LineDevice/DeviceThreads/ThreadXse6.cpp \
    LineModel/CraftParameter.cpp \
    DataHelper/LogHelper.cpp \
    LineDevice/SpectrumAnalyzerDevice/AgilentN9010A.cpp \
    LineDevice/SpectrumAnalyzerDevice/RohdeSchwarz.cpp \
    LineDevice/SpectrumAnalyzerDevice/Tektronix.cpp \
    LineDevice/DeviceThreads/ThreadSpectrumAnalyzer.cpp \
    LineDevice/SpectrumAnalyzerDevice/SpectrumBaseFactory.cpp \
    LineDevice/SpectrumAnalyzerDevice/SpectrumAnalyzerBase.cpp \
    LineDevice/DeviceThreads/ThreadGwMultMet.cpp \
    LineDevice/DeviceThreads/ThreadItechPower.cpp \
    LineDevice/DeviceThreads/ThreadViewCheck.cpp \
    LineDevice/DigitalmeterDevice/SwpC80.cpp \
    LineDevice/MultimeterDevice/GwinstekMultimeter.cpp \
    LineDevice/PlcDevice/DeltaPlc.cpp \
    LineDevice/PowerSupply/ItechPower.cpp \
    LineDevice/ViewCheckDevice/ViewCheck.cpp \
    LineDevice/IOCardDevice/I8O8Card.cpp \
    LineDevice/DeviceThreads/ThreadDeltaPlc.cpp \
    TimeHandler/Timer.cpp \
    LineDevice/PlcDevice/PlcPointInfo.cpp \
    PBOXUI/PlcInfo/PlcControl.cpp \
    PBOXUI/PlcInfo/PlcDisplay.cpp \
    PBOXUI/PlcInfo/PlcPointWidget.cpp \
    PBOXUI/PlcInfo/PlcSetting.cpp \
    PBOXUI/PlcInfo/PlcState.cpp \
    LineModel/LineInformation.cpp \
    main.cpp \
    LineDevice/DeviceThreads/ThreadTpmsHandle.cpp \
    LineModel/ParseLineInfo.cpp \
    LineModel/ProdutcInfo.cpp \
    DataHelper/ParseCsv.cpp \
    PBOXUI/display.cpp \
    PBOXUI/frminput.cpp \
    PBOXUI/mainwidget.cpp \
    PBOXUI/passwordwidget.cpp \
    PBOXUI/PBoxConfig.cpp \
    PBOXUI/TestFrame.cpp \
    PBOXUI/UsrChoose.cpp \
    PBOXUI/PlcInfo/PlcAddItemDialog.cpp \
    PBOXUI/PlcInfo/PlcModifyItem.cpp \
    PBOXUI/PlcInfo/PlcPointBase.cpp \
    PBOXUI/PlcInfo/PlcValue.cpp \
    LineDevice/DigitalmeterDevice/Xse6Tcp.cpp \
    LineDevice/DigitalmeterDevice/Xse6Serial.cpp \
    LineDriver/AlarmDriver.cpp \
    LineDevice/DigitalmeterDevice/Micrometer.cpp \
    LineDevice/ViewCheckDevice/MarkViewCheck.cpp \
    LineDevice/DeviceThreads/ThreadMicrometer.cpp \
    LineDevice/DeviceThreads/ThreadMarkViewCheck.cpp \
    LineDevice/DeviceThreads/ThreadFx5U.cpp \
    LineDevice/PlcDevice/Fx5U.cpp \
    LineDevice/PlcDevice/PlcBase.cpp \
    DataHelper/DatabaseInfo.cpp \
    DataHelper/ArithmeticHelper.cpp \
    DataHelper/LogFile.cpp \
    DataHelper/WatchDogHelper.cpp \
    LineDevice/TpmsHandleDevice/TpmsHandleTcp.cpp \
    LineDevice/DigitalmeterDevice/MicrometerTcp.cpp \
    LineDevice/PowerSupply/ItechPowerTcp.cpp \
    LineDevice/DigitalmeterDevice/Microcaliper.cpp \
    LineDevice/DeviceThreads/ThreadMicrocaliper.cpp \
    LineDevice/DigitalmeterDevice/MicrocaliperTcp.cpp \
    DataHelper/LocalLineInfo.cpp \
    DataCommunication/TcpWorkThread.cpp \
    PBOXUI/UserControl/QMyComboBox.cpp \
    LineDevice/DeviceThreads/ThreadTekDMM6500.cpp \
    LineDevice/MultimeterDevice/TekDMM6500Multimeter.cpp

HEADERS  += \
    LineModel/DeviceConfig.h \
    LineDriver/DeviceDriver.h \
    LineDevice/Device/ComDevice.h \
    LineDevice/Device/DeviceThread.h \
    LineDevice/Device/SerialPortParameters.h \
    LineDevice/Device/TcpDevice.h \
    LineDevice/Device/UdpDevice.h \
    LineDevice/DeviceThreads/ThreadLaserMarker.h \
    LineDevice/MarkerDevice/LaserMarker.h \
    LineDevice/PlcDevice/FxPlc.h \
    LineDevice/ProgramDevice/CycloneProgram.h \
    LineDevice/ProgramDevice/CycloneProUdp.h \
    DataHelper/MemoryDBHelper.h \
    DataConvert/DataType.h \
    LineDevice/DeviceThreads/ThreadFxPlc.h \
    LineModel/ConfigLoader.h \
    LineDevice/Device/ModBus.h \
    LineDevice/PlcDevice/DagongPlc.h \
    LineDevice/DeviceThreads/ThreadDgPlc.h \
    LineDevice/MultimeterDevice/AgilentMultimeterTcp.h \
    LineDevice/MultimeterDevice/AgilentMultimeterSerial.h \
    LineDevice/DeviceThreads/ThreadAgilent.h \
    DataHelper/MySQLHelper.h \
    LineModel/UserInfo.h \
    LineModel/CraftParameter.h \
    LineModel/ProductInfo.h \
    LineModel/libxml/c14n.h \
    LineModel/libxml/catalog.h \
    LineModel/libxml/chvalid.h \
    LineModel/libxml/debugXML.h \
    LineModel/libxml/dict.h \
    LineModel/libxml/DOCBparser.h \
    LineModel/libxml/encoding.h \
    LineModel/libxml/entities.h \
    LineModel/libxml/globals.h \
    LineModel/libxml/hash.h \
    LineModel/libxml/HTMLparser.h \
    LineModel/libxml/HTMLtree.h \
    LineModel/libxml/list.h \
    LineModel/libxml/nanoftp.h \
    LineModel/libxml/nanohttp.h \
    LineModel/libxml/parser.h \
    LineModel/libxml/parserInternals.h \
    LineModel/libxml/pattern.h \
    LineModel/libxml/relaxng.h \
    LineModel/libxml/SAX.h \
    LineModel/libxml/SAX2.h \
    LineModel/libxml/schemasInternals.h \
    LineModel/libxml/schematron.h \
    LineModel/libxml/threads.h \
    LineModel/libxml/tree.h \
    LineModel/libxml/uri.h \
    LineModel/libxml/valid.h \
    LineModel/libxml/xinclude.h \
    LineModel/libxml/xlink.h \
    LineModel/libxml/xmlautomata.h \
    LineModel/libxml/xmlerror.h \
    LineModel/libxml/xmlexports.h \
    LineModel/libxml/xmlIO.h \
    LineModel/libxml/xmlmemory.h \
    LineModel/libxml/xmlmodule.h \
    LineModel/libxml/xmlreader.h \
    LineModel/libxml/xmlregexp.h \
    LineModel/libxml/xmlsave.h \
    LineModel/libxml/xmlschemas.h \
    LineModel/libxml/xmlschemastypes.h \
    LineModel/libxml/xmlstring.h \
    LineModel/libxml/xmlunicode.h \
    LineModel/libxml/xmlversion.h \
    LineModel/libxml/xmlwriter.h \
    LineModel/libxml/xpath.h \
    LineModel/libxml/xpathInternals.h \
    LineModel/libxml/xpointer.h \
    DataCommunication/TcpServer.h \
    LineDevice/DeviceThreads/ThreadDaMaSR.h \
    LineDevice/DeviceThreads/ThreadKeyenceSR.h \
    LineDevice/DeviceThreads/ThreadWenglorSR.h \
    LineDevice/ScannerDevice/DataManSR.h \
    LineDevice/ScannerDevice/KeyenceSR.h \
    LineDevice/ScannerDevice/WenglorSR.h \
    LineDriver/LogicalDriver.h \
    LineDevice/DeviceThreads/ThreadAgilent.h \
    LineDevice/TpmsHandleDevice/TpmsHandle.h \
    LineDevice/DeviceThreads/ThreadXse6.h \
    LineDevice/SpectrumAnalyzerDevice/AgilentN9010A.h \
    LineDevice/SpectrumAnalyzerDevice/RohdeSchwarz.h \
    LineDevice/SpectrumAnalyzerDevice/Tektronix.h \
    LineDevice/DeviceThreads/ThreadSpectrumAnalyzer.h \
    LineDevice/SpectrumAnalyzerDevice/SpectrumAnalyzerBase.h \
    LineDevice/SpectrumAnalyzerDevice/SpectrumBaseFactory.h \
    DataHelper/LogHelper.h \
    LineDevice/DeviceThreads/ThreadGwMultMet.h \
    LineDevice/DeviceThreads/ThreadItechPower.h \
    LineDevice/DeviceThreads/ThreadViewCheck.h \
    LineDevice/DigitalmeterDevice/SwpC80.h \
    LineDevice/MultimeterDevice/GwinstekMultimeter.h \
    LineDevice/PlcDevice/DeltaPlc.h \
    LineDevice/PowerSupply/ItechPower.h \
    LineDevice/ViewCheckDevice/ViewCheck.h \
    LineDevice/IOCardDevice/I8O8Card.h \
    LineDevice/DeviceThreads/ThreadDeltaPlc.h \
    TimeHandler/Timer.h \
    LineDevice/PlcDevice/PlcPointInfo.h \
    PBOXUI/PlcInfo/PlcControl.h \
    PBOXUI/PlcInfo/PlcDisplay.h \
    PBOXUI/PlcInfo/PlcPointWidget.h \
    PBOXUI/PlcInfo/PlcSetting.h \
    PBOXUI/PlcInfo/PlcState.h \
    LineModel/LineInformation.h \
    LineModel/ParseLineInfo.h \
    LineDevice/DeviceThreads/ThreadTpmsHandle.h \
    DataHelper/ParseCsv.h \
    PBOXUI/display.h \
    PBOXUI/frminput.h \
    PBOXUI/mainwidget.h \
    PBOXUI/passwordwidget.h \
    PBOXUI/PBoxConfig.h \
    PBOXUI/TestFrame.h \
    PBOXUI/ui_frminput.h \
    PBOXUI/UsrChoose.h \
    PBOXUI/PlcInfo/PlcAddItemDialog.h \
    PBOXUI/PlcInfo/PlcModifyItem.h \
    PBOXUI/PlcInfo/PlcPointBase.h \
    PBOXUI/PlcInfo/PlcValue.h \
    LineDevice/DigitalmeterDevice/Xse6Tcp.h \
    LineDevice/DigitalmeterDevice/Xse6Serial.h \
    LineDriver/AlarmDriver.h \
    LineDevice/DigitalmeterDevice/Micrometer.h \
    LineDevice/ViewCheckDevice/MarkViewCheck.h \
    LineDevice/DeviceThreads/ThreadMicrometer.h \
    LineDevice/DeviceThreads/ThreadMarkViewCheck.h \
    LineDevice/DeviceThreads/ThreadFx5U.h \
    LineDevice/PlcDevice/Fx5U.h \
    LineDevice/PlcDevice/PlcBase.h \
    DataHelper/DatabaseInfo.h \
    DataHelper/ArithmeticHelper.h \
    DataHelper/LogFile.h \
    DataHelper/WatchDogHelper.h \
    LineDevice/TpmsHandleDevice/TpmsHandleTcp.h \
    LineDevice/DigitalmeterDevice/MicrometerTcp.h \
    LineDevice/PowerSupply/ItechPowerTcp.h \
    LineDevice/DigitalmeterDevice/Microcaliper.h \
    LineDevice/DeviceThreads/ThreadMicrocaliper.h \
    LineDevice/DigitalmeterDevice/MicrocaliperTcp.h \
    main.h \
    DataHelper/LocalLineInfo.h \
    DataCommunication/TcpWorkThread.h \
    LineDevice/DeviceThreads/ThreadCyclone.h \
    PBOXUI/UserControl/QMyComboBox.h \
    LineDevice/DeviceThreads/ThreadTekDMM6500.h \
    LineDevice/MultimeterDevice/TekDMM6500Multimeter.h

FORMS    += \
    PBOXUI/frminput.ui
OTHER_FILES += \
    LineDevice/Device/ComDevice.o \
    LineDevice/Device/DeviceThread.o \
    LineDevice/Device/TcpDevice.o \
    LineDevice/Device/UdpDevice.o
