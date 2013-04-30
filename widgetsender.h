#ifndef WIDGETSENDER_H
#define WIDGETSENDER_H

#include "page.h"
#include <QFile>

namespace Ui {
class widgetSender;
}

class QListWidgetItem;
class QFile;
class rsPacket;
class widgetSender : public Page
{
    Q_OBJECT
    
public:
    explicit widgetSender(QWidget *parent = 0);
    ~widgetSender();
    
private slots:
    void on_spinSize_valueChanged(int arg1);
    void on_butSend_clicked();
    void on_butSave_clicked();
    void on_spinType_valueChanged(int arg1);
    void on_spinAddress_valueChanged(int arg1);
    void on_listPackets_itemClicked(QListWidgetItem *item);

private:
    Ui::widgetSender *ui;
    void reloadPacket();

    void SaveToFile(rsPacket * pack);
    void ReloadFile();
    QFile & fn;

    rsPacket * packet;
    rsPacket * list;
    bool plonk;
};

#endif // WIDGETSENDER_H
