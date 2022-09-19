#ifndef OPERATION_H
#define OPERATION_H

#include <QFuture>
#include <QFutureWatcher>
#include <QList>
#include <QMap>
#include <QWidget>
#include "../steps/step.h"

class Operation : public QWidget
{
    Q_OBJECT
public:
    explicit Operation(const QString &id, const QString &name, QMap<QString, QString> args = (QMap<QString, QString>()), QWidget *parent = 0);
    virtual ~Operation();

    enum Status {Status_Unknown, Status_UserSkipped, Status_Pending, Status_Running, Status_AutoSkipped, Status_Complited, Status_Error};
    enum Result {Result_Unknown, Result_Ok, Result_Warning, Result_Error};

public slots:
    void addStep(Step *step);
    QMap<QString, QString> args();
    QString id();
    virtual void interrupt();
    virtual void launch() = 0;
    QString name();
    void removeStep(Step *step);
    virtual void reset();
    Result result();
    void setResult(Result result);
    void setStatus(Status status);
    Status status();
    QList<Step*> steps();

signals:
    void errorMessage(const QString &text);
    void finished();
    void outputMessage(const QString &text);
    void resultChanged(Operation::Result result);
    void statusChanged(Operation::Status status);

protected:
    QMap<QString, QString> _args;
    QFuture<QMap<QString, int>> _getHostsLoadFuture;
    QFutureWatcher<QMap<QString, int>> _getHostsLoadWatcher;
    QString _id;
    QString _hostName;
    QString _name;
    Result _result;
    Status _status;
    QList<Step*> _steps;

protected slots:
    QString getHostName();
    QMap<QString,int> getHostsLoad();
    void initHostsLoad();
    virtual void onStepResultChanged();
    virtual void onStepStatusChanged();
    void updateResult();
    void updateStatus();

};

#endif // OPERATION_H
