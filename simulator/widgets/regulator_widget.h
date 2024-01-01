#ifndef REGULATOR_WIDGET_H
#define REGULATOR_WIDGET_H

#include "widgets/rgb_led_widget.h"

namespace Ui {
class regulator_widget;
}

namespace GUI {

class CRegulatorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CRegulatorWidget(QWidget *parent = nullptr);
    ~CRegulatorWidget();

public:
    void sensor1LedRedOn   (){SensorLed1_->switchOnRed   ();}
    void sensor1LedBlueOn  (){SensorLed1_->switchOnBlue  ();}
    void sensor1LedGreenOn (){SensorLed1_->switchOnGreen ();}
    void sensor1LedRedOff  (){SensorLed1_->switchOffRed  ();}
    void sensor1LedBlueOff (){SensorLed1_->switchOffBlue ();}
    void sensor1LedGreenOff(){SensorLed1_->switchOffGreen();}

    void sensor2LedRedOn   (){SensorLed2_->switchOnRed   ();}
    void sensor2LedBlueOn  (){SensorLed2_->switchOnBlue  ();}
    void sensor2LedGreenOn (){SensorLed2_->switchOnGreen ();}
    void sensor2LedRedOff  (){SensorLed2_->switchOffRed  ();}
    void sensor2LedBlueOff (){SensorLed2_->switchOffBlue ();}
    void sensor2LedGreenOff(){SensorLed2_->switchOffGreen();}

    void mainLedRedOn      (){WorkingLed_->switchOnRed   ();}
    void mainLedBlueOn     (){WorkingLed_->switchOnBlue  ();}
    void mainLedGreenOn    (){WorkingLed_->switchOnGreen ();}
    void mainLedRedOff     (){WorkingLed_->switchOffRed  ();}
    void mainLedBlueOff    (){WorkingLed_->switchOffBlue ();}
    void mainLedGreenOff   (){WorkingLed_->switchOffGreen();}

    bool getButton1State   ();
    bool getButton2State   ();
private slots:
    void on_btPower_toggled(bool checked);
private:
    Ui::regulator_widget *ui;
    CRgbLedWidget        *SensorLed1_;
    CRgbLedWidget        *SensorLed2_;
    CRgbLedWidget        *WorkingLed_;
    void updateUi();
signals:
    void button1Pressed ();
    void button1Released();
    void button2Pressed ();
    void button2Released();
    void powerOn ();
    void powerOff();
    void releOn  ();
    void releOff ();
};

}//namespace GUI
#endif // REGULATOR_WIDGET_H
