#ifndef WIDGETSENDER_H
#define WIDGETSENDER_H

#include "page.h"
#include <QFile>

namespace Ui {
class widgetSender;
}

class comportClass;
class QListWidgetItem;
class QFile;
class rsPacket;
class widgetSender : public Page
{
    Q_OBJECT
    
public:
    explicit widgetSender(QObject * comic,QWidget *parent = 0);
    ~widgetSender();
    void ReloadLine();
    
private slots:
    void on_spinSize_valueChanged(int arg1);
    void on_butSend_clicked();
    void on_butSave_clicked();
    void on_spinType_valueChanged(int arg1);
    void on_spinAddress_valueChanged(int arg1);
    void on_listPackets_itemClicked(QListWidgetItem *item);
    void timeout();

private:
    Ui::widgetSender *ui;
    void reloadPacket();

    void SaveToFile(rsPacket * pack);
    void ReloadFile();
    void FillSpinList(int count);
    QFile & fn;

    rsPacket * packet;
};

#endif // WIDGETSENDER_H
