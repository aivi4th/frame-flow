#ifndef SERIALOPERATION_H
#define SERIALOPERATION_H

#include "operation.h"

class SerialOperation : public Operation
{
public:
    SerialOperation(const QString &id, const QString &name, QMap<QString, QString> args = (QMap<QString, QString>()), QWidget *parent = 0);
    virtual ~SerialOperation();

public slots:
    void launch();

private slots:
    void onStepStatusChanged();
};

#endif // SERIALOPERATION_H
