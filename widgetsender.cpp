#include "widgetsender.h"
#include "ui_widgetsender.h"
#include "rspacket.h"
#include <QInputDialog>
#include <QFile>

widgetSender::widgetSender(QWidget *parent) :
    Page(parent),
    ui(new Ui::widgetSender)
{
    ui->setupUi(this);

    packet = new rsPacket;
    reloadPacket();
    fn = new QFile("Packet_List.ini");
    ReloadFile();
}

widgetSender::~widgetSender()
{
    delete ui;
}

void widgetSender::on_spinSize_valueChanged(int arg1)
{
    ui->listBytes->clear();

    for (int i = 0 ; i < arg1 ; i++)
    {
        QSpinBox * box = new QSpinBox(this);
        box->setMinimum(0);
        box->setMaximum(255);
        connect(box,SIGNAL(valueChanged(int)),this,SLOT(on_spinType_valueChanged(int)));

        ui->listBytes->addItem(QString());
        ui->listBytes->setItemWidget(ui->listBytes->item(i),box);

    }
    reloadPacket();
}

void widgetSender::reloadPacket()
{
    packet->setAddress(ui->spinAddress->value());
    packet->setCommand(ui->spinType->value());
    packet->setDataSize(ui->spinSize->value());

    for (int i = 0 ; i < ui->spinSize->value(); i++)
    {
        QListWidgetItem * item = ui->listBytes->item(i);
        QSpinBox * box = qobject_cast<QSpinBox*>(ui->listBytes->itemWidget(item));
        packet->setData(i,box->value());
    }

    ui->lineEdit->setText(packet->getFormated());
}

void widgetSender::on_butSend_clicked()
{
    packet->Send();
}

void widgetSender::on_butSave_clicked()
{
    bool ok;
    QString nejm = QInputDialog::getText(this,trUtf8("New packet"),tr("Name"),
                                         QLineEdit::Normal,tr("packet"),&ok);

    if (nejm.isEmpty())
        return;

    QListWidgetItem * item = new QListWidgetItem(nejm);
    packet->name = nejm;
    ui->listPackets->addItem(item);

    item->setData(Qt::UserRole, QVariant::fromValue(packet));

    SaveToFile(packet);
}

void widgetSender::SaveToFile(rsPacket * pack)
{
    if(!fn->open(QFile::WriteOnly,QFile::Append))
        return;
    fn->write(pack->Serialize());
}

void widgetSender::ReloadFile()
{

}

void widgetSender::on_spinType_valueChanged(int )
{
    reloadPacket();
}

void widgetSender::on_spinAddress_valueChanged(int )
{
    reloadPacket();
}
