#include "dataconfstep.h"
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlError>

DataConfStep::DataConfStep(const QString &id, const QString &name, const QMap<QString, QString> &args, QWidget *parent) :
    Step(id, name, args, parent)
{

}

DataConfStep::~DataConfStep()
{

}

void DataConfStep::launch()
{
    setStatus(Status::Status_Checking);
    if (QFile::exists(QString(_args.value("flowDir")).append("/CONFIG/data.conf")) && !_args.keys().contains("force")) {
        setStatus(Status::Status_AutoSkipped);
        setResult(Result::Result_Ok);
        return;
    }

    setStatus(Status::Status_Running);
    QSettings psowConfig(QString(_args.value("flowDir")).append("/CONFIG/data.conf"), QSettings::IniFormat);
    psowConfig.setValue("baseRoutes", _args.value("baseRoutes").split(", "));
    QSqlQuery query;
    QString queryText = "SELECT DISTINCT Layer.Layer, PsowLine.PM_Tonality \
            FROM PsowLine, Psow, BaseRoute, Layer, LayerMask \
            WHERE BaseRoute.Name IN ( \"" + _args.value("baseRoutes").split(", ").join("\", \"") + "\" ) \
            AND Psow.BaseRouteId = BaseRoute.Id \
            AND PsowLine.PsowId = Psow.Id \
            AND LayerMask.LayerId = Layer.Id \
            AND PsowLine.MaskLayerId = LayerMask.Id";
    if (query.exec(queryText)) {
        for (int i = 0; query.next(); ++i) {
            psowConfig.beginGroup(QString::number(i));
            psowConfig.setValue("maskLayer", query.value(0));
            psowConfig.setValue("tone", query.value(1));
            psowConfig.endGroup();
        }
    }
    setStatus(Status::Status_Complited);

    if (query.lastError().type() == QSqlError::NoError && QFile(QString(_args.value("flowDir")).append("/CONFIG/data.conf")).size()) {
        setResult(Result::Result_Ok);
    } else {
        setResult(Result::Result_Error);
    }
}

void DataConfStep::launch(const QString &)
{
    launch();
}
