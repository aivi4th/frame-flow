#ifndef FLOW_H
#define FLOW_H

#include <QList>
#include "stages/stage.h"

class Flow : public QWidget
{
    Q_OBJECT

public:
    explicit Flow(const QString &dirPath, QWidget *parent = 0);
    ~Flow();

    enum Status {Status_UserSkipped, Status_Pending, Status_Running, Status_AutoSkipped, Status_Complited, Status_Error, Status_Unknown};
    enum Result {Result_Unknown, Result_Ok, Result_Warning, Result_Error};
    
public slots:
    void addStage(Stage* stage);
    QString dirPath();
    void initialize();
    void interrupt();
    void launch();
    void loadLastConfiguration();
    void onBaseRoutesListChanged(const QStringList &baseRoutes);
    void onDirPathChanged(const QString &dirPath);
    void reset();
    Result result();
    void setBaseRoutesList(const QStringList &baseRoutes);
    void setDirPath(const QString &dirPath);
    void setResult(Result result);
    void setStatus(Status status);
    QList<Stage*> stages();
    Status status();

signals:
    void resultChanged(Flow::Result result);
    void statusChanged(Flow::Status status);

private:
    QStringList _baseRoutes;
    QString _dirPath;
    Result _result;
    QList<Stage*> _stages;
    Status _status;

private slots:
    void onStageResultChanged(Stage::Result result);
    void onStageStatusChanged(Stage::Status status);
    void updateResult();
    void updateStatus();
};

#endif // FLOW_H
