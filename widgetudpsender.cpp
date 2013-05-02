#include "widgetudpsender.h"
#include "ui_widgetudpsender.h"
#include "widgetsender.h"
#include "rspacket.h"


widgetUdpSender::widgetUdpSender(wifiPacketUdp * udp,QWidget *parent) :
    Page(parent),
    ui(new Ui::widgetUdpSender)
{
    ui->setupUi(this);
    packet = new wifiPacket_t;
     widget = new widgetSender(udp,this);
    widget->setProperty("neco",QVariant::fromValue(packet));

    ui->verticalLayout->addWidget(widget);

    packet->ID = ui->spinID->value();
    packet->cmd = ui->spinCommand->value();
}

widgetUdpSender::~widgetUdpSender()
{
    delete ui;
}

void widgetUdpSender::on_spinID_valueChanged(int arg1)
{
    packet->ID = arg1;
    widget->ReloadLine();
}

void widgetUdpSender::on_spinCommand_valueChanged(int arg1)
{
    packet->cmd = arg1;
    widget->ReloadLine();
}
