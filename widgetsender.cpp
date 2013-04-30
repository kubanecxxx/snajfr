#include "widgetsender.h"
#include "ui_widgetsender.h"
#include "rspacket.h"
#include <QInputDialog>
#include <QFile>

widgetSender::widgetSender(QWidget *parent) :
    Page(parent),
    ui(new Ui::widgetSender),
    fn(*new QFile("List.ini",this))
{
    ui->setupUi(this);

    packet = new rsPacket;
    reloadPacket();
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
    plonk = true;
}

void widgetSender::on_butSend_clicked()
{
    if (plonk)
        packet->Send();
    else
        list->Send();

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

    packet = new rsPacket(*packet);
    reloadPacket();
}

void widgetSender::SaveToFile(rsPacket * pack)
{
    fn.close();
    if(!fn.open(QFile::WriteOnly |QFile::Append))
    {
        fn.close();
        if(!fn.open(QFile::WriteOnly))
        {
            fn.close();
            return;
        }
    }
    fn.write(pack->Serialize());
    fn.write("\n");
    fn.close();
}

void widgetSender::ReloadFile()
{
    if(!fn.open(QFile::ReadOnly))
    {
        fn.close();
        return;
    }

    ui->listPackets->clear();
    QByteArray temp;
    temp = fn.readLine();
    while(temp.length())
    {
        rsPacket * pack = new rsPacket;
        pack->Unserialize(temp);

        QListWidgetItem * item = new QListWidgetItem(pack->name);
        ui->listPackets->addItem(item);
        item->setData(Qt::UserRole, QVariant::fromValue(pack));

        temp = fn.readLine();
    }

    fn.close();
}

void widgetSender::on_spinType_valueChanged(int )
{
    reloadPacket();
}

void widgetSender::on_spinAddress_valueChanged(int )
{
    reloadPacket();
}

void widgetSender::on_listPackets_itemClicked(QListWidgetItem *item)
{
    rsPacket * pack = item->data(Qt::UserRole).value<rsPacket*>();
    ui->lineEdit->setText(pack->getFormated());

    list = pack;
    plonk = false;
}
