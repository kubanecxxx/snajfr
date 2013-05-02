#ifndef WIDGETUDP_H
#define WIDGETUDP_H

#include "page.h"

namespace Ui {
class widgetUdp;
}

class wifiPacketUdp;
class widgetUdp : public Page
{
    Q_OBJECT
    
public:
    explicit widgetUdp(wifiPacketUdp * upd,QWidget *parent = 0);
    ~widgetUdp();
    
private slots:
    void on_pushOpen_clicked();

    void on_pushClose_clicked();

private:
    Ui::widgetUdp *ui;
    wifiPacketUdp * udp;
};

#endif // WIDGETUDP_H
