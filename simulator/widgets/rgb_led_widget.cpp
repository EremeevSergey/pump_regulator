#include <QPainter>
#include "rgb_led_widget.h"

namespace GUI {

CRgbLedWidget::CRgbLedWidget(QWidget *parent)
    : QWidget{parent}
    , Color_{0,0,0}
{

}


void CRgbLedWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    if (isEnabled())
        painter.setBrush({Color_});
    else
        painter.setBrush({Qt::gray});
    QRect r{2,2,size().width()-4,size().height()-4};
    painter.drawEllipse(r);
}

}//namespace GUI
