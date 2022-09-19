#ifndef STEP_H
#define STEP_H

#include <QFileDevice>
#include <QMap>
#include <QWidget>

class Step : public QWidget
{
    Q_OBJECT

public:
    explicit Step(const QString &id, const QString &name, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget *parent = 0);
    virtual ~Step();

    enum Status {Status_UserSkipped, Status_Pending, Status_Launching, Status_Checking, Status_AutoSkipped, Status_Running, Status_Complited, Status_Error};
    enum Result {Result_Unknown, Result_Ok, Result_Warning, Result_Error};
    
public slots:
    QMap<QString, QString> args();
    QString id();
    virtual void interrupt();
    virtual void launch() = 0;
    virtual void launch(const QString &hostName) = 0;
    QString logPath();
    QString name();
    virtual void reset();
    Result result();
    void setArg(const QString &key, const QString &value);
    void setResult(Result result);
    void setStatus(Status status);
    Status status();

signals:
    void errorMessage(const QString &message);
    void logPathChanged(const QString &logPath);
    void outputMessage(const QString &message);
    void resultChanged(Step::Result result);
    void statusChanged(Step::Status status);

protected:
    QMap<QString, QString> _args;
    QString _id;
    QString _name;
    QString _logPath;
    QFileDevice::Permissions _permissions;
    Result _result;
    Status _status;

protected slots:
    void setLogPath(const QString &logPath);
};

#endif // STEP_H
