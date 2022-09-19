#ifndef SCRIPTSTEP_H
#define SCRIPTSTEP_H

#include <QMap>
#include <QProcess>
#include "step.h"

class ScriptStep : public Step
{
    Q_OBJECT

public:
    ScriptStep(const QString &id, const QString &name, const QString &interpretator, const QString &script, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget *parent = 0);
    virtual ~ScriptStep();

public slots:
    virtual void interrupt();
    virtual void launch();
    virtual void launch(const QString &hostName);

protected:
    QString _interpretator;
    QProcess _process;
    QString _script;

protected slots:
    void onProcessFinished(const int &exitCode);
    void onProcessReadyReadStandardError();
    void onProcessReadyReadStandardOutput();
};

#endif // SCRIPTSTEP_H
