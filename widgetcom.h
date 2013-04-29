#ifndef WIDGETCOM_H
#define WIDGETCOM_H

#include <page.h>

namespace Ui {
class WidgetCom;
}

class comportClass;
class WidgetCom : public Page
{
    Q_OBJECT
    
public:
    explicit WidgetCom(comportClass * comik,QWidget *parent = 0);
    ~WidgetCom();
    
private slots:
    void on_butRefresh_clicked();
    void on_butOpen_clicked();
    void on_butClose_clicked();
    void comport_state(bool);

private:
    Ui::WidgetCom *ui;
    comportClass *com;
};

#endif // WIDGETCOM_H
