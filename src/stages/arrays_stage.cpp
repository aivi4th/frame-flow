#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include "arrays_stage.h"
#include "arrays_config.h"
#include "../operations/operationfactory.h"
#include "../steps/stepfactory.h"



// Constructor
Arrays_stage::Arrays_stage(const QString &id, const QString &name, const QString &flowDirPath, const QMap<QString, QString> &args, QWidget *parent) :
    SerialStage(id, name, flowDirPath, args, parent)
{
    setConfig(new Arrays_config(name));
}



// Destructor
Arrays_stage::~Arrays_stage()
{

}



// Initializes stage before launch
void Arrays_stage::initialize()
{
    Operation* operation;
    Step* step;
    QMap<QString, QString> stepArgs;
    Arrays_config* config = qobject_cast<Arrays_config*>(_config);

    if (config) {

        const QStringList arrays = config->arrays();
        const QStringList checkedArrays = config->checkedArrays();
        const QStringList plts = config->plts();
        const QStringList checkedPlts = config->checkedPlts();
        const QStringList layers = config->layers();
        const QStringList checkedLayers = config->checkedLayers();
        const QMap<QString,QString> mergingProgram = config->mergingFiles();
        const QString dataConfigPath = config->dataConfigPath();
        const QString configFilePath = config->configFilePath();
        const QString mergingFilePath = config->mergingFilePath();

        QJsonObject stageGroup;
        stageGroup.insert("checked", this->isChecked());
        stageGroup.insert("forced", this->isForced());
        stageGroup.insert("cad", config->isCadChecked());
        stageGroup.insert("mask", config->isMaskChecked());
        stageGroup.insert("config", config->configFilePath());

        int i = 0;
        foreach (QString array, arrays) {
            QJsonObject arrayGroup;
            arrayGroup.insert("name", array);
            arrayGroup.insert("checked", checkedArrays.contains(array));
            stageGroup.insert(QString("Array_").append(QString::number(++i)), arrayGroup);
        }
        i = 0;
        foreach (QString plt, plts) {
            QJsonObject pltGroup;
            pltGroup.insert("name", plt);
            pltGroup.insert("checked", checkedPlts.contains(plt));
            stageGroup.insert(QString("Plt_").append(QString::number(++i)), pltGroup);
        }
        i = 0;
        foreach (QString layer, layers) {
            QJsonObject layerGroup;
            layerGroup.insert("name", layer);
            layerGroup.insert("checked", checkedLayers.contains(layer));
            layerGroup.insert("merging", mergingProgram.value(layer));
            stageGroup.insert(QString("Layer_").append(QString::number(++i)), layerGroup);
        }

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

        // Cadarrays operation
        operation = OperationFactory::createOperation("cadarrays", QMap<QString, QString>(), this);
        if (operation) {

            // Cadarray steps
            foreach (QString array, arrays) {
                stepArgs.clear();
                if (this->isForced()) {
                    stepArgs["force"] = QString();
                }
                stepArgs["flowDir"] = _flowDirPath;
                stepArgs["config"] = configFilePath;
                stepArgs["array"] = array;
                step = StepFactory::createStep("cadarray", stepArgs, operation);
                if (step) {
                    if (!this->isChecked() || !config->isCadChecked() || !checkedArrays.contains(array)) {
                        step->setStatus(Step::Status_UserSkipped);
                    }
                    operation->addStep(step);
                }
            }

            // Cadplt steps
            foreach (QString plt, plts) {
                stepArgs.clear();
                if (this->isForced()) {
                    stepArgs["force"] = QString();
                }
                stepArgs["flowDir"] = _flowDirPath;
                stepArgs["config"] = configFilePath;
                stepArgs["plt"] = plt;
                step = StepFactory::createStep("cadplt", stepArgs, operation);
                if (step) {
                    if (!this->isChecked() || !config->isCadChecked() || !checkedPlts.contains(plt)) {
                        step->setStatus(Step::Status_UserSkipped);
                    }
                    operation->addStep(step);
                }
            }

            addOperation(operation);
        }

        // Maskarrays operation
        operation = OperationFactory::createOperation("maskarrays", QMap<QString, QString>(), this);
        if (operation) {

            // Maskarray steps
            foreach (QString array, arrays) {
                stepArgs.clear();
                if (this->isForced()) {
                    stepArgs["force"] = QString();
                }
                stepArgs["flowDir"] = _flowDirPath;
                stepArgs["array"] = array;
                stepArgs["merging"] = mergingFilePath;
                stepArgs["layers"] = checkedLayers.join(" ");
                stepArgs["dataConfig"] = dataConfigPath;
                step = StepFactory::createStep("maskarray", stepArgs, operation);
                if (step) {
                    if (!this->isChecked() || !config->isMaskChecked() || !checkedArrays.contains(array) || checkedLayers.isEmpty()) {
                        step->setStatus(Step::Status_UserSkipped);
                    }
                    operation->addStep(step);
                }
            }

            // Maskplt steps
            foreach (QString plt, plts) {
                stepArgs.clear();
                if (this->isForced()) {
                    stepArgs["force"] = QString();
                }
                stepArgs["flowDir"] = _flowDirPath;
                stepArgs["plt"] = plt;
                stepArgs["merging"] = mergingFilePath;
                stepArgs["layers"] = checkedLayers.join(" ");
                stepArgs["dataConfig"] = dataConfigPath;
                step = StepFactory::createStep("maskplt", stepArgs, operation);
                if (step) {
                    if (!this->isChecked() || !config->isMaskChecked() || !checkedPlts.contains(plt) || checkedLayers.isEmpty()) {
                        step->setStatus(Step::Status_UserSkipped);
                    }
                    operation->addStep(step);
                }
            }

            addOperation(operation);
        }

        updateStatus();
    }
}



