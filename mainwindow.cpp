#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgetcom.h"
#include "rspacket.h"
#include "hled.h"
#include "widgetsender.h"
#include "widgetreceiver.h"
#include "widgetudp.h"
#include "widgetudpsender.h"
#include <QTreeWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    com = new comportClass(this);

    HLed * led = new HLed;
    led->turnOff();
    connect(com,SIGNAL(connectionStatus(bool)),led,SLOT(turnOn(bool)));
    statusBar()->addPermanentWidget(led);

    QTreeWidgetItem * it;

    WidgetCom * Com = new WidgetCom(com,this);

    it = ui->centralWidget->AddPage(NULL);

    it->setText(0,tr("Com"));
    ui->centralWidget->AddPage(Com,it);
    ui->centralWidget->AddPage(new widgetSender(com,this),it);
    //ui->centralWidget->AddPage(new widgetReceiver(com,this));

    it = ui->centralWidget->AddPage(NULL);
    it->setText(0,tr("UDP"));

    wifiPacketUdp * udp = new wifiPacketUdp(this);
    connect(udp,SIGNAL(connectionStatus(bool)),led,SLOT(turnOn(bool)));

    ui->centralWidget->AddPage(new widgetUdp(udp,this),it);
    ui->centralWidget->AddPage(new widgetUdpSender(udp,this),it);


    it->treeWidget()->expandAll();
}

MainWindow::~MainWindow()
{
    delete ui;
}
