#ifndef UTILS_H
#define UTILS_H
#include <QAction>
#include <QWidget>

void setWidgetTo   (QWidget* what,QWidget* where);

QAction* newAction(const QString& name, const char* icon_name,
                   QObject* receiver,const char* method);

#endif // UTILS_H
