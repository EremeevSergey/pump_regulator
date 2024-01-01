#ifndef RGB_LED_WIDGET_H
#define RGB_LED_WIDGET_H

#include <QWidget>

namespace GUI {

class CRgbLedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CRgbLedWidget(QWidget *parent = nullptr);

    void switchOnRed   (){Color_.setRed  (255); update();}
    void switchOffRed  (){Color_.setRed  (0  ); update();}

    void switchOnGreen (){Color_.setGreen(255); update();}
    void switchOffGreen(){Color_.setGreen(0  ); update();}

    void switchOnBlue  (){Color_.setBlue (255); update();}
    void switchOffBlue (){Color_.setBlue (0  ); update();}
protected:
    virtual void paintEvent(QPaintEvent *event) override;
private:
    QColor Color_;
};

}//namespace GUI
#endif // RGB_LED_WIDGET_H
