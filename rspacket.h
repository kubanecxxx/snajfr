#ifndef RSPACKET_H
#define RSPACKET_H

#include <stdint.h>
typedef struct delay_t;
#include "rs485.h"
#include <QByteArray>
#include <QObject>
#include <QVariant>
#include <QUdpSocket>


class wifiPacket_t;
class QTimer;
class comportClass;
class QIODevice;
class rsPacket
{
public:
    rsPacket();
    rsPacket(const rsPacket & cpy);
    ~rsPacket();

    void Send() const ;

    QByteArray getRaw(bool neco = true) const;
    QString getFormated() const;

    uint8_t calculateChecksum(void) const;

    void setAddress(uint8_t add){p.address = add;}
    void setDataSize(uint8_t size);
    void setData(uint8_t idx, uint8_t data);
    void setCommand(uint8_t comm) {p.command = static_cast<cmd_t>(comm);}
    QByteArray Serialize();
    void Unserialize(const QByteArray & str);
    void fromRaw(QByteArray & data);

    QString name;
    const packet_t * getPacket() {return &p;}
    uint8_t getRealChecksum() const {return internalChecksum;}
    void SetPar(wifiPacket_t * par){parent = par;}

private:
    packet_t p;

    void Send(QIODevice * device) const;
    static QIODevice * dev;
    uint8_t internalChecksum;
    bool TryFill(QByteArray & data);

    wifiPacket_t * parent;
    static void SetupDevice(QIODevice * device ){dev = device;}
    friend class comportClass;
    friend class wifiPacket_t;
};

/******************************************************
 */

class QextSerialPort;
class comportClass : public QObject
{
    Q_OBJECT

public:
    comportClass(QObject * parent = NULL);
    ~comportClass();

    void Close();
    bool Open(const QString & port, const QString & speed = "9600");
    QStringList GetPortList();
    QStringList GetPortSpeeds();

private:
    QextSerialPort * com;
    rsPacket * packet;
    QByteArray data;
    QTimer * timer;

private slots:
    void NewData();
    void abToclose() {emit connectionStatus(false);}
    void timeout();
    void readData();

signals:
    void NewPacket(rsPacket * packet);
    void connectionStatus(bool);
};

/******************************************************
 */

class wifiPacket_t
{

public:
    wifiPacket_t();
    wifiPacket_t(wifiPacket_t & cpy);
    void Send();
    uint8_t ID;
    uint8_t cmd;
    rsPacket * p;
    QByteArray getRaw();
    QString getFormated();

private:
    bool TryFill(QByteArray & data);
    friend class wifiPacketUdp;
    static QUdpSocket * dev;
    static int port;
};

class wifiPacketUdp : public QObject
{
    Q_OBJECT
public:
    wifiPacketUdp(QObject * par = NULL);
    void Open(int port);
    void Close();
    ~wifiPacketUdp() {wifiPacket_t::dev = NULL;}

private:
    QUdpSocket * socket;
    int port;
    QByteArray buffer;
    wifiPacket_t * packet;

private slots:
    void newData();
    void newDataTimeout();

signals:
    void NewPacket(rsPacket * packet);
    void connectionStatus(bool);
};

Q_DECLARE_METATYPE(rsPacket *)
Q_DECLARE_METATYPE(wifiPacket_t *)

#endif // RSPACKET_H
