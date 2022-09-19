#ifndef WORKDIR_STAGE_H
#define WORKDIR_STAGE_H

#include "parallelstage.h"

class WorkDir_stage : public ParallelStage
{
public:
    WorkDir_stage(const QString &id, const QString &name, const QString &flowDirPath, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget *parent = 0);
    ~WorkDir_stage();

public slots:
    void initialize();
    void setArgs(QString args);
};

#endif // WORKDIR_STAGE_H
