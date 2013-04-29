#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "widgetcom.h"
#include "rspacket.h"
#include "hled.h"
#include "widgetsender.h"

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


    WidgetCom * Com = new WidgetCom(com,this);
    ui->centralWidget->AddPage(Com);
    ui->centralWidget->AddPage(new widgetSender(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}
