#ifndef DATACONFSTEP_H
#define DATACONFSTEP_H

#include <QMap>
#include "step.h"

class DataConfStep : public Step
{
    Q_OBJECT

public:
    DataConfStep(const QString &id, const QString &name, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget *parent = 0);
    virtual ~DataConfStep();

public slots:
    void launch();
    void launch(const QString &);
};

#endif // DATACONFSTEP_H
