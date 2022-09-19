#ifndef FRAME_STAGE_H
#define FRAME_STAGE_H

#include "parallelstage.h"

class Frame_stage : public ParallelStage
{
public:
    Frame_stage(const QString &id, const QString &name, const QString &flowDirPath, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget *parent = 0);
    ~Frame_stage();

public slots:
    void initialize();
};

#endif // FRAME_STAGE_H
