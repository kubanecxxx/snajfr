#include "widgetreceiver.h"
#include "ui_widgetreceiver.h"
#include <QTreeWidget>
#include "rspacket.h"
#include <QTime>

widgetReceiver::widgetReceiver(QObject * comic,QWidget *parent) :
    Page(parent),
    ui(new Ui::widgetReceiver)
{
    ui->setupUi(this);
    connect(comic,SIGNAL(NewPacket(rsPacket*)),this,SLOT(newPacket(rsPacket*)));
    if (!comic->property("srot").isValid())
        ui->treeAll->hideColumn(2);
}

widgetReceiver::~widgetReceiver()
{
    delete ui;
}

void widgetReceiver::on_treeAll_itemClicked(QTreeWidgetItem *item, int)
{
    ui->treeOne->clear();
    rsPacket * pack = item->data(0,Qt::UserRole).value<rsPacket*>();

    QTreeWidgetItem * root= new QTreeWidgetItem;
    root->setText(0,tr("Packet"));
    root->setText(1,pack->getFormated()+ QString(" (%1)").
                  arg(pack->getRealChecksum(),0,16));

    QTreeWidgetItem * child;

    child = new QTreeWidgetItem;
    child->setText(0,tr("Address"));
    child->setText(1,QString(" 0x%1").
                   arg(pack->getPacket()->address,0,16));
    root->addChild(child);

    child = new QTreeWidgetItem;
    child->setText(0,tr("Data Size"));
    child->setText(1,QString(" 0x%1").
                   arg(pack->getPacket()->dataSize,0,16));
    root->addChild(child);

    child = new QTreeWidgetItem;
    child->setText(0,tr("Command"));
    child->setText(1,QString(" 0x%1").
                   arg(pack->getPacket()->command,0,16));
    root->addChild(child);

    child = new QTreeWidgetItem;
    child->setText(0,tr("Data"));
    for (int i = 0 ;i < pack->getPacket()->dataSize; i++)
    {
        QTreeWidgetItem * it = new QTreeWidgetItem;
        it->setData(0,Qt::DisplayRole,i);
        it->setText(1,QString(" 0x%1").
                       arg(pack->getPacket()->data[i],0,16));
        child->addChild(it);
    }
    root->addChild(child);

    child = new QTreeWidgetItem;
    child->setText(0,tr("Checksum"));
    child->setText(1,QString(" 0x%1").
                   arg(pack->getRealChecksum(),0,16));
    root->addChild(child);

    ui->treeOne->addTopLevelItem(root);
    ui->treeOne->expandAll();
}

void widgetReceiver::newPacket(rsPacket * packet)
{
    QTreeWidgetItem * item = new QTreeWidgetItem;
    ui->treeAll->addTopLevelItem(item);

    item->setData(0,Qt::DisplayRole,QTime::currentTime());

    item->setText(1,packet->getFormated() + QString(" (%1)").
                  arg(packet->getRealChecksum(),0,16));
    item->setData(0,Qt::UserRole,QVariant::fromValue(packet));


    if (packet->Parent() && packet->Parent()->ipValid())
        item->setText(2,packet->Parent()->Ip().toString());
}
