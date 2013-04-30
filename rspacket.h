#ifndef RSPACKET_H
#define RSPACKET_H

#include <stdint.h>
#include "rs485.h"
#include <QByteArray>
#include <QObject>
#include <QVariant>

class comportClass;
class QIODevice;
class rsPacket
{
public:
    rsPacket();
    rsPacket(const rsPacket & cpy);
    ~rsPacket();

    void Send() const {Send(dev);}

    QByteArray getRaw() const;
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

private:
    packet_t p;

    void Send(QIODevice * device) const;
    static QIODevice * dev;

    static void SetupDevice(QIODevice * device ){dev = device;}
    friend comportClass;
};

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

private slots:
    void NewData();
    void abToclose() {emit connectionStatus(false);}

signals:
    void NewPacket(rsPacket packet);
    void connectionStatus(bool);
};

Q_DECLARE_METATYPE(rsPacket *)

#endif // RSPACKET_H
