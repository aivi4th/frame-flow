#ifndef OPERATIONFACTORY_H
#define OPERATIONFACTORY_H

#include <QMap>
#include "operation.h"

class OperationFactory
{
public:
    static Operation* createOperation(const QString &id, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget* parent = 0);
};

#endif // OPERATIONFACTORY_H
