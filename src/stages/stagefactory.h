#ifndef STAGEFACTORY_H
#define STAGEFACTORY_H

#include "stage.h"

class StageFactory
{
    public:
    static Stage* createStage(const QString &id, const QString &flowDirPath, const QMap<QString, QString> &args = (QMap<QString, QString>()), QWidget* parent = 0);
};

#endif // STAGEFACTORY_H
