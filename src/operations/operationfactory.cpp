#include <QDebug>
#include <QFileInfo>
#include "paralleloperation.h"
#include "serialoperation.h"
#include "operationfactory.h"



// Creates operation
Operation* OperationFactory::createOperation(const QString &id, const QMap<QString, QString> &args, QWidget *parent)
{
    QString name;
    if (id == "workdir") {
        name = "Подготовка рабочего каталога";
        return new SerialOperation(id, name, args, parent);
    }else if (id == "cadarrays") {
        name = "Формирование массивов в топологических слоях";
        return new ParallelOperation(id, name, args, parent);
    } else if (id == "maskarrays") {
        name = "Формирование массивов в масочных слоях";
        return new ParallelOperation(id, name, args, parent);
    } else if (id == "cadframe") {
        name = "Формирование фрейма в топологических слоях";
        return new ParallelOperation(id, name, args, parent);
    } else if (id == "maskframe") {
        name = "Формирование фрейма в масочных слоях";
        return new ParallelOperation(id, name, args, parent);
    } else {
        return 0;
    }
}
