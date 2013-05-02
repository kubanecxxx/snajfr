#ifndef WIDGETUDPSENDER_H
#define WIDGETUDPSENDER_H

#include "page.h"
namespace Ui {
class widgetUdpSender;
}

class widgetSender;
class wifiPacket_t;
class wifiPacketUdp;
class widgetUdpSender : public Page
{
    Q_OBJECT
    
public:
    explicit widgetUdpSender(wifiPacketUdp * udp,QWidget *parent = 0);
    ~widgetUdpSender();
    
private slots:
    void on_spinID_valueChanged(int arg1);

    void on_spinCommand_valueChanged(int arg1);

private:
    Ui::widgetUdpSender *ui;
    wifiPacket_t * packet;
    widgetSender * widget;
};

#endif // WIDGETUDPSENDER_H
