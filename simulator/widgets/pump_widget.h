#ifndef PUMP_WIDGET_H
#define PUMP_WIDGET_H

#include <QWidget>

namespace Ui {
class pump_widget;
}

namespace GUI {

class CPumpWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CPumpWidget(QWidget *parent = nullptr);
    ~CPumpWidget();
protected:
    virtual void timerEvent(QTimerEvent *event) override;
private:
    Ui::pump_widget *ui;
    int WorkTimerId_;
    bool Work_;
    void updateUi();
public slots:
    void switchOn (){Work_=true ;updateUi();}
    void switchOff(){Work_=false;updateUi();}
signals:
    void doWork   ();
private slots:
    void on_checkBox_stateChanged(int arg1);
};

}//namespace GUI

#endif // PUMP_WIDGET_H
