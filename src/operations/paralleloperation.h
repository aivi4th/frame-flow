#ifndef PARALLELOPERATION_H
#define PARALLELOPERATION_H

#include "operation.h"
#include <QQueue>

class ParallelOperation : public Operation
{
public:
    ParallelOperation(const QString &id, const QString &name, QMap<QString, QString> args = (QMap<QString, QString>()), QWidget *parent = 0);
    virtual ~ParallelOperation();

public slots:
    void launch();

private slots:
    void onStepStatusChanged();

private:
    QQueue<Step*> _runningSteps;
    Step* _runningStep = nullptr;
};

#endif // PARALLELOPERATION_H
