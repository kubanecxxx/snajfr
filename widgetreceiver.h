#ifndef WIDGETRECEIVER_H
#define WIDGETRECEIVER_H

#include "page.h"
#include "rspacket.h"

class comportClass;
class QTreeWidgetItem;
namespace Ui {
class widgetReceiver;
}

class widgetReceiver : public Page
{
    Q_OBJECT
    
public:
    explicit widgetReceiver(QObject * comic, QWidget *parent = 0);
    ~widgetReceiver();
    
private slots:
    void on_treeAll_itemClicked(QTreeWidgetItem *item, int column);
    void newPacket(rsPacket *);

private:
    Ui::widgetReceiver *ui;
};

#endif // WIDGETRECEIVER_H
