#include "widgetudp.h"
#include "ui_widgetudp.h"
#include "rspacket.h"

widgetUdp::widgetUdp(wifiPacketUdp * udp,QWidget *parent) :
    Page(parent),
    ui(new Ui::widgetUdp)
{
    this->udp = udp;
    ui->setupUi(this);
}

widgetUdp::~widgetUdp()
{
    delete ui;
}

void widgetUdp::on_pushOpen_clicked()
{
    udp->Open(ui->spinPort->value());
}

void widgetUdp::on_pushClose_clicked()
{
    udp->Close();
}
