#include "widgetsender.h"
#include "ui_widgetsender.h"
#include "rspacket.h"
#include <QInputDialog>
#include <QFile>
#include "widgetreceiver.h"
#include <QTimer>

widgetSender::widgetSender(QObject * comic,QWidget *parent) :
    Page(parent),
    ui(new Ui::widgetSender),
    fn(*new QFile("List.ini",this))
{
    ui->setupUi(this);
    ui->verticalLayout->addWidget(new widgetReceiver(comic,this));



    QTimer::singleShot(50,this,SLOT(timeout()));
}

void widgetSender::ReloadLine()
{
        ui->lineEdit->setText(packet->getFormated());
}

void widgetSender::timeout()
{
    packet = new rsPacket;
    if (property("neco").isValid())
        packet->SetPar(property("neco").value<wifiPacket_t*>());

    reloadPacket();
    ReloadFile();
}

widgetSender::~widgetSender()
{
    delete ui;
}

void widgetSender::FillSpinList(int arg1)
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
}

void widgetSender::on_spinSize_valueChanged(int arg1)
{
    FillSpinList(arg1);
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

    ReloadLine();
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

        if (property("neco").isValid())
            pack->SetPar(property("neco").value<wifiPacket_t*>());

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
    Q_ASSERT(pack);

    //load widget
    ui->spinAddress->setValue(pack->getPacket()->address);
    ui->spinType->setValue(pack->getPacket()->command);
    ui->spinSize->setValue(pack->getPacket()->dataSize);

    FillSpinList(ui->spinSize->value());

    for (int i = 0 ; i < ui->spinSize->value(); i++)
    {
        QListWidgetItem * item = ui->listBytes->item(i);
        QSpinBox * box = qobject_cast<QSpinBox*>(ui->listBytes->itemWidget(item));
        box->setValue(pack->getPacket()->data[i]);
    }

    ReloadLine();

    packet = new rsPacket(*pack);
    Q_ASSERT(packet);
}
