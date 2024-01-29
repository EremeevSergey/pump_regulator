#include "pump_widget.h"
#include "ui_pump_widget.h"

namespace GUI {

constexpr int WorkTimerPeriod  = 300; //!< Таймер, по которому синус волны движется

CPumpWidget::CPumpWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::pump_widget)
    , WorkTimerId_{0}
    , Work_{false}
    , invert_{false}
{
    ui->setupUi(this);
    WorkTimerId_=startTimer(WorkTimerPeriod);
    updateUi();
}

CPumpWidget::~CPumpWidget()
{
    killTimer(WorkTimerId_);
    delete ui;
}

void CPumpWidget::timerEvent(QTimerEvent *event)
{
    if (event!=nullptr && event->timerId()==WorkTimerId_){
        bool work = Work_;
        if (invert_) work=!work;
        if (work && ui->checkBox->isChecked())
            emit doWork();
    }
}

void CPumpWidget::on_checkBox_stateChanged(int)
{
    updateUi();
}

void CPumpWidget::updateUi()
{
    bool en = ui->checkBox->isChecked();
    ui->labelState->setEnabled(en);
    bool work = Work_;
    if (invert_) work=!work;
    if (en && work){
        ui->labelState->setStyleSheet("QLabel { background-color : red; color : white; }");
        ui->labelState->setText("Вкл.");
    }
    else{//QPalette::WindowText
        ui->labelState->setStyleSheet("QLabel { background-color : Window; color : WindowText; }");
        ui->labelState->setText("Выкл.");
    }
}


void CPumpWidget::on_cbInvers_stateChanged(int arg1)
{
    if (ui->cbInvers->isChecked()) invert_ = true;
    else invert_=false;
    updateUi();
}

}//namespace GUI
