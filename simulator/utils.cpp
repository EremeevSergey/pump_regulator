#include <QIcon>
#include <QHBoxLayout>
#include "utils.h"

void setWidgetTo   (QWidget* what,QWidget* where)
{
    QHBoxLayout* lay = new QHBoxLayout(where);
    lay->setMargin(0);
    lay->addWidget(what);
    where->setLayout(lay);
}

QAction* newAction(const QString& name, const char* icon_name,
                   QObject* receiver,const char* method)
{
    QAction* act = new QAction(QIcon(QString::fromUtf8(icon_name)),name,receiver);
    QObject::connect(act,SIGNAL(triggered()),receiver,method);
    return act;
}
