#ifndef STEPFACTORY_H
#define STEPFACTORY_H

#include <QMap>
#include "step.h"

class StepFactory
{
public:
    static Step* createStep(const QString &id, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget* parent = 0);
};

#endif // STEPFACTORY_H
