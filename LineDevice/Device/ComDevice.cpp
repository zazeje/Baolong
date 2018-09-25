#include "ComDevice.h"

int ComDevice::baudflag_arr[] =
{
    B921600, B460800, B230400, B115200, B57600, B38400,
    B19200, B9600, B4800, B2400, B1800, B1200,
    B600, B300, B150, B110, B75, B50
};
int ComDevice::speed_arr[] =
{
    921600, 460800, 230400, 115200, 57600, 38400,
    19200, 9600, 4800, 2400, 1800, 1200,
    600, 300, 150, 110, 75, 50
};

ComDevice::ComDevice()
{}

ComDevice::ComDevice(SerialPortParameters* spp,bool blockMode)
{
    isReConnect = false;
    _spp = spp;
    SerialPortInit(_spp->_portName,_spp->_baudRate,_spp->_parity,_spp->_dataBits,_spp->_stopBits,blockMode);
    endStr = "\r\n";
    stopRecon = false;
    isStopflag = false;
    m_communicateStatus = true;
}

ComDevice::ComDevice(SerialPortParameters* spp, string name, bool blockMode)
{
    isReConnect = false;
    Name = name;
    _spp = spp;
    portname = _spp->_portName;
    SerialPortInit(_spp->_portName,_spp->_baudRate,_spp->_parity,_spp->_dataBits,_spp->_stopBits,blockMode);
    endStr = "\r\n";
    stopRecon = false;
    isStopflag = false;
    m_communicateStatus = true;
    _log.LOG_DEBUG("【%s】 端口【%s】:波特率【%d】:数据位【%d】:停止位【%d】:校验位【%d】 已连接，不需要重新连接!",\
                   Name.data(),_spp->_portName.data(),_spp->_baudRate,_spp->_dataBits,_spp->_stopBits,_spp->_parity);

}

ComDevice::~ComDevice()
{
    if(_spp != NULL)
    {
        delete _spp;
        _spp = NULL;
    }
    close(SerialPort_fd);
    isStopflag = true;
}

/**
* <summary> 打开指定串口
* <param>  PortName  指定串口的端口号
* <return>  打开成功返回true，否则返回false
* */
bool ComDevice::Open(string PortName,bool blockMode)
{
    string Device;
    if(!PortName.compare("COM1"))
    {
        Device = "/dev/ttymxc0";
    }
    else if(!PortName.compare("COM2"))
    {
        Device = "/dev/ttymxc1";
    }
    else if(!PortName.compare("COM3"))
    {
        Device = "/dev/ttymxc2";
    }
    else if(!PortName.compare("COM4"))
    {
        Device = "/dev/ttymxc3";
    }
    else if(!PortName.compare("COM5"))
    {
        Device = "/dev/ttymxc4";
    }
    else if(!PortName.compare("COM6"))
    {
        Device = "/dev/ttyUSB0";
    }
    else if(!PortName.compare("COM7"))
    {
        Device = "/dev/ttyUSB1";
    }
    if(blockMode)
    {
        if((SerialPort_fd = open((char*)Device.data(), O_RDWR | O_SYNC /*| O_NONBLOCK*/)) < 0)         //| O_NOCTTY | O_NDELAY
        {
            _log.LOG_ERROR("ComDevice 设备【%s】  端口【%s】 端口打开错误 !",Name.data(),PortName.data());
            return false;
        }
    }
    else
    {
        if((SerialPort_fd = open((char*)Device.data(), O_RDWR | O_SYNC | O_NONBLOCK)) < 0)         //| O_NOCTTY | O_NDELAY
        {
            _log.LOG_ERROR("ComDevice 设备【%s】  端口【%s】 端口打开失败 !",Name.data(),PortName.data());
            return false;
        }
    }
//    _log.LOG_DEBUG("ComDevice 设备【%s】  端口【%s】 打开成功 !",Name.data(),PortName.data());
    return true;
}

/**
* <summary> 关闭串口
* */
void ComDevice::Close()
{    
    if(SerialPort_fd != -1)
    {
        close(SerialPort_fd);
//        _log.LOG_DEBUG("ComDevice 设备【%s】 端口【%s】 端口关闭成功！",Name.data(),_spp->_portName.data());
    }
}

/**
* <summary> 初始化指定串口
* <param>  PortName  指定串口的端口号
* <param>  baudRate  类型 int  串口波特率
* <param>  databits  类型 int  数据位
* <param>  parity  类型 int  校验位
* <param>  stopbits 类型 int 停止位
* <return>  初始化成功返回0，否则返回-1
* */
int ComDevice::SerialPortInit(string PortName,int baudRate,int parity,int dataBits,int stopBits,bool blockMode)
{
    Open(PortName,blockMode);
    return setup_port(SerialPort_fd,baudRate,dataBits,parity,stopBits);
}

void ComDevice::CloseReconThread()
{
    isStopflag = true;
    stopRecon = true;
}

/**
 * @brief 串口断线重连
 * @return
 */
bool ComDevice::threadprocess()
{
//    _log.LOG_INFO("ComDevice 设备【%s】 串口已断开,正在尝试重新连接.... ",Name.data());
    //先将串口关闭
    while(!stopRecon)
    {
        Close();
        if(!isStopflag)
        {
//        _log.LOG_DEBUG("ComDevice 设备【%s】 正在执行断线重连...",Name.data());
        //重新初始化串口
            if(SerialPortInit(_spp->_portName,_spp->_baudRate,_spp->_parity,_spp->_dataBits,_spp->_stopBits))
            {
                stopRecon = true;
                isReConnect = false;
    //            _log.LOG_DEBUG("ComDevice 设备【%s】 断线重连操作成功！",Name.data());
                if(isStopflag)
                {
                    Close();
                }
                break;
            }
            else
            {
                if(isStopflag)
                {
                    Close();
                    break;
                }
            }
            sleep(2);
        }
        else
        {
            break;
        }
    }
    stopRecon = false;
    isReConnect = false;
//    _log.LOG_DEBUG("ComDevice 设备【%s】 已退出重连线程 !",Name.data());
    return true;
}

/**
 * @brief 断线重连线程执行函数
 * (由于线程执行函数必须为静态函数，所以通过代入的指针调用类内方法)
 * @param arg
 * @return
 */
void *ComDevice::Start_Thread(void* arg)
{
    ComDevice *th = (ComDevice*)arg;
    th->threadprocess();
    return NULL;
}

/**
 * @brief 串口断线重连线程启动方法
 */
bool ComDevice::Restart_Serial()
{
    if(!isStopflag)
    {
        if(!isReConnect)
        {
    //        _log.LOG_DEBUG("ComDevice 设备【%s】 重连线程已启动 ... ",Name.data());
            isReConnect = true;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
            int ret = pthread_create(&pth,&attr,ComDevice::Start_Thread,this);
            if(ret != 0)
            {
                isReConnect = false;
                return false;
            }
            else
                return true;
        }
    }
}

bool ComDevice::Stop()
{
    pthread_detach(pth);
    pthread_cancel(pth);
    return true;
}

/**
* <summary> 从串口读取一个字节数据
* <param>  tmp_buff  数据存放缓冲区
* <return>  读取成功返回1，否则返回-1
* */
int ComDevice::ReadByte(void *tmp_buff)
{
    int ret = 0;
    if((ret = read(SerialPort_fd,tmp_buff,1)) <= 0)
    {
//        _log.LOG_ERROR("ComDevice 设备【%s】 向COM端口ReadBytes失败! 当前通信状态为【%d】",Name.data(), m_communicateStatus);
        if(!m_communicateStatus)
            Restart_Serial();
        return false;
    }
    return true;
}

/**
 * @brief 将读取到的数据以ASCII码保存在数组中
 * @return 数据字符串
 */
string ComDevice::ReadAscii()
{
    char tmp_buff[1024] = {0};
    string buff;
    int ret;
    if((ret = read(SerialPort_fd,tmp_buff,1024)) <= 0)
    {
//        _log.LOG_ERROR("ComDevice 设备【%s】 向COM端口ReadAscii【失败】! 当前通信状态为【%d】",Name.data(), m_communicateStatus);
        if(!m_communicateStatus)
            Restart_Serial();
        return "";
    }
    buff=tmp_buff;
    return buff;
}

/**
 * @brief 将读取到的数据转化为16进制字符串
 * @return
 */
string ComDevice::ReadHex()
{
    char tmp_buff[SIZE]={0};
    int ret = 0;
    char m_buff[SIZE] = {0};
    int i = 0;
    int offset = 0;
    string buff;
    if((ret = read(SerialPort_fd,tmp_buff,SIZE)) <= 0)
    {
        _log.LOG_ERROR("ComDevice 设备【%s】 向COM端口ReadHex【失败】 当前通信状态为【%d】",Name.data(), m_communicateStatus);
        if(!m_communicateStatus)
            Restart_Serial();
        return "";
    }
    for(i=0;i < ret;i++)
    {
        offset += sprintf(m_buff+offset,"%02X",tmp_buff[i]);
    }
    buff = m_buff;
    return buff;
}

/**
 * @brief 以ASCII码方式写数据
 * @param 数据内容
 * @return 写入成功返回true
 */
bool ComDevice::WriteAscii(string command)
{       
    int ret;
    if((ret=write(SerialPort_fd,command.data(),command.length())) <= 0)
    {
        _log.LOG_ERROR("ComDevice 设备【%s】 向COM端口WriteAscii【失败】 当前通信状态为【%d】",Name.data() ,m_communicateStatus);
        if(!m_communicateStatus)
            Restart_Serial();
        return false;
    }
    return true;
}

/**
 * @brief 以16进制方式写数据
 * 将需要写入的数据内容转化为16进制字符数组写入
 * @param 写入数据内容
 * @return 写入成功返回true
 */
bool ComDevice::WriteHex(string command)
{
    int ret;
    unsigned char Hex[1024]={0};
    stringtodec(command,Hex);
    if((ret=write(SerialPort_fd,Hex,command.length()/2)) <= 0)
    {
        _log.LOG_ERROR("ComDevice 设备【%s】 向COM端口WriteHex【失败】 当前通信状态为【%d】",Name.data() ,m_communicateStatus);
        if(!m_communicateStatus)
            Restart_Serial();
        return false;
    }
    return true;
}

/**
 * @brief 以ASCII码方式进行读写数据
 * @param 写入的数据
 * @param 写数据与读数据之间的延时时间
 * @return 返回读取的字符串
 */
string ComDevice::WriteAndReadAscii(string command, int delay)
{
    WriteAscii(command);
    usleep(delay * 1000);
    return ReadAscii();
}

/**
 * @brief 以16进制方式读写数据
 * @param 写入的数据
 * @param 写数据与读数据之间的延时时间
 * @return 返回读取的字符串
 */
string ComDevice::WriteAndReadHex(string command, int delay)
{
    WriteHex(command);
    usleep(delay * 1000);
    return ReadHex();
}

/**
 * @brief 写数据
 * @param 写入的数据内容
 * @param 写入数据的方式
 */
bool ComDevice::Write(string command, string charType)
{
    if(!command.empty())
    {
        if(!charType.compare("Ascii"))
            return WriteAscii(command);
        else if(!charType.compare("Hex"))
            return WriteHex(command);
    }
    return false;
}

/**
 * @brief 读数据
 * @param 读取数据的方式
 */
string ComDevice::Read(string charType)
{
    string result;
    if(!charType.compare("Ascii"))
        result = ReadAscii();
    else if(!charType.compare("Hex"))
        result = ReadHex();

    return result;
}

/**
 * @brief 读写数据
 * @param 写入数据内容
 * @param 读写数据方式
 * @param 写数据与读数据之间延时
 * @return 读取的数据
 */
string ComDevice::WriteAndRead(string command, string charType, int delay)
{
    string result = "";
    if(!command.empty() && !isReConnect)
    {
        if(!charType.compare("Ascii"))
        {
            result = WriteAndReadAscii(command,delay);
        }
        else if(!charType.compare("Hex"))
            result = WriteAndReadHex(command,delay);
    }
    return result;
}

/**
 * @brief 以ASCII方式写数据，以16进制方式读数据
 * @param 写入数据内容
 * @param 延时时间
 */
string ComDevice::WriteAsciiAndReadHex(string command, int delay)
{
    string result = "";
    if(!command.empty() && !isReConnect)
    {
        WriteAscii(command);
        usleep(delay * 1000);
        result = ReadHex();
    }
    return result;
}

/**
 * @brief 以16进制方式写数据，以ASCII方式读数据
 * @param 写入数据内容
 * @param 延时时间
 */
string ComDevice::WriteHexAndReadAscii(string command, int delay)
{
    string result = "";
    if(!command.empty() && !isReConnect)
    {
        WriteHex(command);
        usleep(delay * 1000);
        result = ReadAscii();
    }
    return result;
}

/**
 * @brief 获取串口通信句柄
 */
int ComDevice::GetSerialPortfd()
{
    return SerialPort_fd;
}

/**
* <summary> 设置串口通信
* <param>  fd     类型 int  打开串口的文件句柄
* <param>  baud  类型 int  串口波特率
* <param>  databits  类型 int  数据位
* <param>  parity  类型 int  校验位
* <param>  stopbits 类型 int 停止位
* <return>  设置成功返回0，否则返回-1
* */
bool ComDevice::setup_port(int fd, int baud, int databits, int parity, int stopbits)
{
    struct termio term_attr;
    struct termio oterm_attr;

    //Get current setting
    if (ioctl(fd, TCGETA, &term_attr) < 0) {
        return -1;
    }

    // Backup old setting
    memcpy(&oterm_attr, &term_attr, sizeof(struct termio));

    term_attr.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    term_attr.c_oflag &= ~(OPOST | ONLCR | OCRNL);
    term_attr.c_lflag &= ~(ISIG | ECHO | ICANON | NOFLSH);
    term_attr.c_cflag &= ~CBAUD;
    term_attr.c_cflag |= CREAD | speed_to_flag(baud);

    //Set databits
    term_attr.c_cflag &= ~(CSIZE);
    switch (databits) {
        case 5:
            term_attr.c_cflag |= CS5;
            break;

        case 6:
            term_attr.c_cflag |= CS6;
            break;

        case 7:
            term_attr.c_cflag |= CS7;
            break;

        case 8:
        default:
            term_attr.c_cflag |= CS8;
            break;
    }

    //Set parity
    switch (parity) {
        case odd:       //Odd parity 奇校验
            term_attr.c_cflag |= (PARENB | PARODD);
            break;

        case even:      //Even parity 偶校验
            term_attr.c_cflag |= PARENB;
            term_attr.c_cflag &= ~(PARODD);
            break;

        case none:      //None parity
        default:
            term_attr.c_cflag &= ~(PARENB);
            break;
    }


    //Set stopbits
    switch (stopbits) {
        case 2:     //2 stopbits
            term_attr.c_cflag |= CSTOPB;
            break;

        case 1:     //1 stopbits
        default:
            term_attr.c_cflag &= ~CSTOPB;
            break;
    }

    term_attr.c_cc[VMIN] = 0;
    term_attr.c_cc[VTIME] = 15;

    if (ioctl(fd, TCSETAW, &term_attr) < 0) {
        return false;
    }

    if (ioctl(fd, TCFLSH, 2) < 0) {
        return false;
    }
    return true;
}

/**
 * @brief 获取波特率
 */
int ComDevice::speed_to_flag(int speed)
{
    unsigned int i;

    for (i = 0; i < sizeof(speed_arr)/sizeof(int); i++) {
        if (speed == speed_arr[i])
        {
            return baudflag_arr[i];
        }
    }
    return B9600;
}
