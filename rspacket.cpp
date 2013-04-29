#include "rspacket.h"
#include <string.h>
#include <stdlib.h>
#include <QIODevice>
#include "qextserialport.h"
#include <QStringList>
#include "qextserialenumerator.h"

QIODevice * rsPacket::dev = NULL;

rsPacket::rsPacket()
{
    memset(&p,0,sizeof(p));
}

rsPacket::rsPacket(const rsPacket & cpy)
{
    memcpy(this,&cpy,sizeof(rsPacket));

    p.data = (uint8_t *) malloc(p.dataSize);
    memcpy(p.data,cpy.p.data,sizeof(p.dataSize));
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

void rsPacket::Send(QIODevice *device) const
{
    if (device == NULL)
        return;

    QByteArray arr = getRaw();
    if (device->isWritable())
        device->write(arr);
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

    str = getRaw().toHex();

    int i = str.length();
    str.insert(i -= 2, " ");
    str.insert(i -= p.dataSize *2 , " ");
    str.insert(i -= 2, " ");
    str.insert(i -= 2, " ");
    str.insert(i -= 2, " ");

    return str;
}

QByteArray rsPacket::getRaw() const
{
    QByteArray temp;

    temp.append(0xAA);
    temp.append(p.address);
    temp.append(p.dataSize);
    temp.append(p.command);

    for (int i = 0 ;i < p.dataSize; i++)
        temp.append(p.data[i]);

    temp.append(calculateChecksum());

    return temp;
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

void comportClass::NewData()
{

}

QStringList comportClass::GetPortSpeeds()
{
    if (list.isEmpty())
        list << "9600" << "19200";

    return list;

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


