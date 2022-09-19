#ifndef CALIBRESTEP_H
#define CALIBRESTEP_H

#include "scriptstep.h"

class CalibreStep : public ScriptStep
{
public:
    CalibreStep(const QString &id, const QString &name, const QString &interpretator, const QString &script, const QMap<QString, QString> &args, QWidget *parent = 0);
    virtual ~CalibreStep();

public slots:
    void launch();
    void launch(const QString &hostName);

protected slots:
    QMap<QString, QString> calibreVariables(const QString &calibreVersion);
};

#endif // CALIBRESTEP_H
