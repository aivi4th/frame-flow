#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include "workdir_stage.h"
#include "workdir_config.h"
#include "../operations/operationfactory.h"
#include "../steps/stepfactory.h"



// Constructor
WorkDir_stage::WorkDir_stage(const QString &id, const QString &name, const QString &flowDirPath, const QMap<QString, QString> &args, QWidget *parent) :
    ParallelStage(id, name, flowDirPath, args, parent)
{
    setConfig(new WorkDir_config(name));
}



// Destructor
WorkDir_stage::~WorkDir_stage()
{

}



// Initializes stage before launch
void WorkDir_stage::initialize()
{
    Operation* operation;
    Step* step;
    QMap<QString, QString> stepArgs;
    WorkDir_config* config = qobject_cast<WorkDir_config*>(_config);

    if (config) {

        const QString baseFramePath = config->baseFramePath();

        QJsonObject stageGroup;
        stageGroup.insert("checked", this->isChecked());
        stageGroup.insert("forced", this->isForced());
        stageGroup.insert("base", baseFramePath);

        QJsonObject mainGroup;
        QString jsonString;
        if (QFileInfo(QApplication::instance()->property("runConfigPath").toString()).exists()) {
            QFile runConfig(QApplication::instance()->property("runConfigPath").toString());
            runConfig.open(QIODevice::ReadOnly | QIODevice::Text);
            jsonString = runConfig.readAll();
            runConfig.close();
            QJsonDocument runConf = QJsonDocument::fromJson(jsonString.toUtf8());
            mainGroup = runConf.object();
        }
        mainGroup.insert(this->id(), stageGroup);

        QJsonDocument runConf;
        runConf.setObject(mainGroup);
        jsonString = runConf.toJson(QJsonDocument::Indented);

        QFile runConfig;
        runConfig.setFileName(QApplication::instance()->property("runConfigPath").toString());
        runConfig.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream stream(&runConfig);
        stream << jsonString;
        runConfig.close();

        // Workdir operation
        operation = OperationFactory::createOperation("workdir", QMap<QString, QString>(), this);
        if (operation) {

            // Workdir step
            stepArgs.clear();
            if (this->isForced()) {
                stepArgs["force"] = QString();
            }
            stepArgs["flowDir"] = _flowDirPath;

            step = StepFactory::createStep("pcg", stepArgs, operation);
            if (!this->isChecked()) {
                step->setStatus(Step::Status_UserSkipped);
            }
            operation->addStep(step);

            // Frame2cells step
            if (!baseFramePath.isEmpty()) {
                stepArgs.clear();
                if (this->isForced()) {
                    stepArgs["force"] = QString();
                }
                stepArgs["flowDir"] = _flowDirPath;
                stepArgs["base"] = baseFramePath;
                step = StepFactory::createStep("frame2cells", stepArgs, operation);
                if (!this->isChecked()) {
                    step->setStatus(Step::Status_UserSkipped);
                }
                operation->addStep(step);
            }

            // DataConfig step
            stepArgs.clear();
            if (this->isForced()) {
                stepArgs["force"] = QString();
            }
            stepArgs["flowDir"] = _flowDirPath;
            stepArgs["baseRoutes"] = _baseRoutes.join(", ");

            step = StepFactory::createStep("dataconf", stepArgs, operation);
            if (!this->isChecked()) {
                step->setStatus(Step::Status_UserSkipped);
            }
            operation->addStep(step);

            addOperation(operation);
        }
    }

    updateStatus();
}







