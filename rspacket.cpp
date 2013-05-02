#include "rspacket.h"
#include <string.h>
#include <stdlib.h>
#include <QIODevice>
#include "qextserialport.h"
#include <QStringList>
#include "qextserialenumerator.h"
#include <QDebug>
#include <QTimer>
#include <QNetworkInterface>

QIODevice * rsPacket::dev = NULL;

rsPacket::rsPacket()
{
    memset(&p,0,sizeof(p));
    parent = NULL;
}

rsPacket::rsPacket(const rsPacket & cpy)
{
    memcpy(this,&cpy,sizeof(rsPacket));

    if (p.dataSize)
    p.data = (uint8_t *) malloc(p.dataSize);
    if (p.dataSize)
        memcpy(p.data,cpy.p.data,p.dataSize);
}

rsPacket::~rsPacket()
{
    if (p.data)
        free(p.data);
}

uint8_t rsPacket::calculateChecksum() const
{
    uint8_t temp;

    temp = p.address;
    temp += p.dataSize;
    temp += p.command;

    for (int i = 0 ;i < p.dataSize; i++)
    {
        temp += p.data[i];
    }
    temp ^= 0b01010101;

    return temp;
}

void rsPacket::Send()  const
{
    Send(dev);
}

void rsPacket::Send(QIODevice *device) const
{
    if (parent)
    {
        parent->Send();
        return;
    }

    QByteArray arr = getRaw();
    qDebug() << arr.toHex();
    if (device == NULL)
        return;

    if (device->isWritable())
    {
        device->write(arr);

    }
}

void rsPacket::setData(uint8_t idx, uint8_t data)
{
    if (idx > p.dataSize)
        return;

    p.data[idx]  = data;
}

void rsPacket::setDataSize(uint8_t size)
{
    p.dataSize = size;
    if (p.data == NULL)
        p.data = (uint8_t *) malloc(size);
    else
        p.data = (uint8_t *) realloc(p.data,size);
}

QString rsPacket::getFormated() const
{
    QString str;

    if (parent)
        str = parent->getFormated();
    str += getRaw(false).toHex();

    int i = str.length();
    str.insert(i -= 2, " ");
    str.insert(i -= p.dataSize *2 , " ");
    str.insert(i -= 2, " ");
    str.insert(i -= 2, " ");
    str.insert(i -= 2, " ");

    return str;
}

QByteArray rsPacket::getRaw(bool neco) const
{
    QByteArray temp;

    if (neco)
        if (parent)
            temp = parent->getRaw();

    temp.append(0xAA);
    temp.append(p.address);
    temp.append(p.dataSize);
    temp.append(p.command);

    for (int i = 0 ;i < p.dataSize; i++)
        temp.append(p.data[i]);

    temp.append(calculateChecksum());

    return temp;
}

void rsPacket::fromRaw(QByteArray &data)
{
    uint8_t pre = data.at(0);
    data.remove(0,1);

    p.address = data.at(0);
    data.remove(0,1);
    p.dataSize = data.at(0);
    data.remove(0,1);
    p.command = static_cast<cmd_t>(data.at(0));
    data.remove(0,1);
    if (p.dataSize)
        p.data = (uint8_t *)malloc(p.dataSize);
    for (int i = 0; i < p.dataSize; i++)
    {
        p.data[i] = data.at(i);
    }
}

QByteArray rsPacket::Serialize()
{
    QByteArray temp = name.toAscii();
    temp += " ";
    temp += getRaw().toHex();
    return temp;
}

void rsPacket::Unserialize(const QByteArray  &str)
{
    int i = str.indexOf(" ");
    QByteArray name = str.left(i);
    QByteArray temp = str.right(str.length() - i);
    QByteArray arr = QByteArray::fromHex(temp.trimmed());

    this->name = QString::fromAscii(name);
    fromRaw(arr);
}

bool rsPacket::TryFill(QByteArray &data)
{
    if (data.count() < 5)
        return false;


    while((uint8_t)data.at(0) != 0xAA)
        data.remove(0,1);

    p.address = data.at(1);
    p.dataSize = data.at(2);
    p.command = static_cast<cmd_t>(data.at(3));

    if (data.count() < 5 + p.dataSize)
        return false;

    int i = 0 ;
    if (p.dataSize)
        p.data = (uint8_t*) malloc(p.dataSize);
    for ( i = 0 ; i < p.dataSize; i++)
    {
        p.data[i] = data.at(i + 4);

    }
    internalChecksum = data.at(i+4);

    data.remove(0,5+p.dataSize);

    return true;
}

/******************************************************
 */

comportClass::comportClass(QObject * parent):
    QObject(parent)
{
    PortSettings nastaveni;
    nastaveni.BaudRate = BAUD1152000;
    nastaveni.DataBits = DATA_8;
    nastaveni.FlowControl = FLOW_OFF;
    nastaveni.Parity = PAR_NONE;
    nastaveni.StopBits = STOP_1;
    nastaveni.Timeout_Millisec = 100;

    com = new QextSerialPort(nastaveni,QextSerialPort::EventDriven,this);

    rsPacket::SetupDevice(com);
    connect(com,SIGNAL(readyRead()),this,SLOT(NewData()));
    connect(com,SIGNAL(aboutToClose()),this,SLOT(abToclose()));

    packet = new rsPacket;

    timer = new QTimer;
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    timer->setSingleShot(true);
}

comportClass::~comportClass()
{
    rsPacket::SetupDevice(NULL);
}

void comportClass::Close()
{
    com->close();
    emit connectionStatus(false);
}

static QStringList list;
BaudRateType types[] =
{
    BAUD9600, BAUD19200
};

bool comportClass::Open(const QString & port, const QString & speed)
{
    com->close();

    com->setBaudRate(types[list.indexOf(speed)]);
    com->setPortName(port);
    bool state =com->open(QIODevice::ReadWrite);

    emit connectionStatus(state);
    return state;
}

void comportClass::readData()
{
    data += com->readAll();
    timer->start(100);

    while (packet->TryFill(data))
    {
        rsPacket * temp = new rsPacket(*packet);
        emit NewPacket(temp);
    }
}

void comportClass::NewData()
{
    QTimer::singleShot(10,this,SLOT(readData()));
}

QStringList comportClass::GetPortSpeeds()
{
    if (list.isEmpty())
        list << "9600" << "19200";

    return list;

}

void comportClass::timeout()
{
    data.clear();
}

QStringList comportClass::GetPortList()
{
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
      QStringList list;

      for (int i = 0 ; i < ports.count() ; i++)
      {

      #ifdef Q_OS_WIN
           list << ports.at(i).portName;
      #else
          list << ports.at(i).physName;

      #endif

      }

      #ifndef Q_OS_WIN
      list << "/tmp/interceptty";
      #endif

      return list;
}

/******************************************************
 */

QUdpSocket * wifiPacket_t::dev = NULL;
int wifiPacket_t::port;

wifiPacket_t::wifiPacket_t()
{
    ID=0;
    cmd=0;
    p = new rsPacket;
    p->parent = this;
}

wifiPacket_t::wifiPacket_t(wifiPacket_t & cpy)
{
    ID = cpy.ID;
    cmd = cpy.cmd;
    p = new rsPacket(*cpy.p);
    p->parent = this;
}

void wifiPacket_t::Send()
{
    QHostAddress addr;
    addr = QString("192.168.1.%1").arg((int)ID);
    QString tmp = addr.toString();
    qDebug() << tmp;

    if (dev && dev->isWritable())
    {
        QByteArray temp = p->getRaw();
        int j = dev->writeDatagram(temp,addr,port);
        asm("nop");
    }

}

QByteArray wifiPacket_t::getRaw()
{
    QByteArray temp;
    temp.append(0xEE);
    temp.append(ID);
    temp.append(cmd);

    return temp;
}

QString wifiPacket_t::getFormated()
{
    QString str = getRaw().toHex();
    int i = str.length();
    str.insert(i -= 2, " ");
    str.insert(i -= 2, " ");
    str.append(" | ");

    return str;
}

bool wifiPacket_t::TryFill(QByteArray &data)
{
    if (data.count() < 8)
        return false;


    while((uint8_t)data.at(0) != 0xEE)
        data.remove(0,1);

    ID = data.at(1);
    cmd = data.at(2);

    QByteArray temp = data.left(3);
    data.remove(0,3);
    bool ok = p->TryFill(data);

    if (ok)
    {
        return true;
    }
    else
    {
        data.prepend(temp);
        return false;
    }
}

/******************************************************
 */

wifiPacketUdp::wifiPacketUdp(QObject *par):
    QObject(par)
{
    socket = new QUdpSocket(this);

    connect(socket,SIGNAL(readyRead()),this,SLOT(newData()));
    wifiPacket_t::dev = socket;
    packet = new wifiPacket_t;
}

void wifiPacketUdp::Open(int port)
{
    wifiPacket_t::port = port;
    this->port = port;
  //  socket->bind(QHostAddress::Any,port);
    bool ok = socket->open(QIODevice::ReadWrite);
    emit connectionStatus(ok);
}

void wifiPacketUdp::Close()
{
    socket->close();
    connectionStatus(false);
}

void wifiPacketUdp::newData()
{
    QTimer::singleShot(20,this,SLOT(newDataTimeout()));
}

void wifiPacketUdp::newDataTimeout()
{
    buffer += socket->readAll();
    if (packet->TryFill(buffer))
    {
        wifiPacket_t *pac = new wifiPacket_t(*packet);
        emit NewPacket(pac->p);
    }
}

