#include "widgetcom.h"
#include "ui_widgetcom.h"
#include "rspacket.h"

WidgetCom::WidgetCom(comportClass * comic, QWidget *parent) :
    Page(parent),
    ui(new Ui::WidgetCom),
    com(comic)
{
    ui->setupUi(this);
    on_butRefresh_clicked();

    connect(com,SIGNAL(connectionStatus(bool)),this,SLOT(comport_state(bool)));
    comport_state(false);
}

WidgetCom::~WidgetCom()
{
    delete ui;
}

void WidgetCom::on_butRefresh_clicked()
{
    ui->comboCOM->clear();
    ui->comboCOM->addItems(com->GetPortList());

    ui->comboSpeed->clear();
    ui->comboSpeed->addItems(com->GetPortSpeeds());
}

void WidgetCom::on_butOpen_clicked()
{
    com->Open(ui->comboCOM->currentText(),ui->comboSpeed->currentText());
}

void WidgetCom::on_butClose_clicked()
{
    com->Close();
}

void WidgetCom::comport_state(bool state)
{
    ui->butClose->setEnabled(state);
    ui->butOpen->setDisabled(state);
    ui->butRefresh->setDisabled(state);

    ui->comboCOM->setDisabled(state);
    ui->comboSpeed->setDisabled(state);
}
