#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QTimer>
#include "flow.h"
#include "stages/stagefactory.h"



// Constructor 1
Flow::Flow(const QString &dirPath, QWidget *parent) :
    QWidget(parent),
    _dirPath(dirPath)
{
    QSettings commonConfig(QFileInfo(QApplication::applicationDirPath()).dir().path().append("/cfg/common.conf"), QSettings::NativeFormat);
    Stage* stage;
    foreach (QString id, commonConfig.value("stages").toStringList()) {
        stage = StageFactory::createStage(id, dirPath, QMap<QString, QString>(), this);
        addStage(stage);
    }
    reset();
}



// Destructor
Flow::~Flow()
{

}



// Adds stage to the flow
void Flow::addStage(Stage* stage)
{
    connect(stage, SIGNAL(statusChanged(Stage::Status)), this, SLOT(onStageStatusChanged(Stage::Status)));
    connect(stage, SIGNAL(resultChanged(Stage::Result)), this, SLOT(onStageResultChanged(Stage::Result)));
    _stages << stage;
    updateStatus();
    updateResult();
}



// Returns flow directory path
QString Flow::dirPath()
{
    return _dirPath;
}



// Initializes flow before launch
void Flow::initialize()
{
    foreach (Stage* stage, _stages) {
        stage->initialize();
    }
}



// Interupts flow
void Flow::interrupt()
{
    foreach(Stage* stage, _stages) {
        stage->interrupt();
    }
    reset();
}



// Launches flow
void Flow::launch()
{
    foreach(Stage* stage, _stages) {
        if (stage->status() == Stage::Status_Pending) {
            stage->launch();
            break;
        }
    }
}


void Flow::loadLastConfiguration()
{
    const QString logDirPath = QString(_dirPath).append("/LOGS");
    const QStringList runConfigNames = QDir(logDirPath).entryList(QStringList("run_*.conf"), QDir::Files, QDir::Name);
    if (!runConfigNames.isEmpty()) {
        const QString lastRunConfigPath = logDirPath + "/" + runConfigNames.last();

        QString jsonString;
        QFile runConfig(lastRunConfigPath);
        runConfig.open(QIODevice::ReadOnly | QIODevice::Text);
        jsonString = runConfig.readAll();
        runConfig.close();

        QJsonDocument runConf = QJsonDocument::fromJson(jsonString.toUtf8());
        QJsonObject mainGroup = runConf.object();

        foreach(Stage* stage, _stages) {
            QJsonValue stageConfig = mainGroup.value(stage->id());
            if (stageConfig.isObject()) {
                QJsonObject stageGroup = stageConfig.toObject();
                stage->setChecked(stageGroup["checked"].toBool());
                stage->setForced(stageGroup["forced"].toBool());
                runConf.setObject(stageGroup);
                stage->setArgs(runConf.toJson());
            }
        }
    }
}



void Flow::onBaseRoutesListChanged(const QStringList &baseRoutes)
{
    foreach(Stage* stage, _stages) {
        stage->setBaseRoutesList(baseRoutes);
    }

}


// Do, when flow directory path is changed
void Flow::onDirPathChanged(const QString &dirPath)
{
    foreach(Stage* stage, _stages) {
        stage->setFlowDirPath(dirPath);
    }

    QTimer::singleShot(100, this, SLOT(loadLastConfiguration()));
    loadLastConfiguration();
}



// Do, when stage result is changed
void Flow::onStageResultChanged(Stage::Result result)
{
    Q_UNUSED(result);
    updateResult();
}



// Do, when stage status is changed
void Flow::onStageStatusChanged(Stage::Status status)
{
    Q_UNUSED(status);
    Stage* prevStage = qobject_cast<Stage*>(sender());
    if (prevStage) {
        Stage::Status prevStatus = prevStage->status();
        Stage::Result prevResult = prevStage->result();

        if ((prevStatus == Stage::Status_Complited || prevStatus == Stage::Status_AutoSkipped) && prevResult != Stage::Result_Error && prevResult != Stage::Result_Unknown && prevStage != _stages.last()) {
            int prevIndex = _stages.indexOf(prevStage);
            if (prevIndex != -1) {
                Stage* nextStage;
                for (int nextIndex = prevIndex + 1; nextIndex < _stages.size(); nextIndex++) {
                    nextStage = _stages[nextIndex];
                    if (nextStage->status() == Stage::Status_Pending) {
                        nextStage->launch();
                        break;
                    }
                }
            }
        }
    }




    /*
    if (_status == Status_Running) {
        Stage* prevStage = qobject_cast<Stage*>(sender());
        if (prevStage) {
            Stage::Status prevStatus = prevStage->status();
            Stage::Result prevResult = prevStage->result();
            if ((prevStatus == Stage::Status_Complited || prevStatus == Stage::Status_AutoSkipped) && prevResult != Stage::Result_Error && prevResult != Stage::Result_Unknown && prevStage != _stages.last()) {
                int prevIndex = _stages.indexOf(prevStage);
                if (prevIndex != -1) {
                    Stage* nextStage;
                    for (int nextIndex = prevIndex + 1; nextIndex < _stages.size(); nextIndex++) {
                        nextStage = _stages[nextIndex];
                        if (nextStage->status() == Stage::Status_Pending) {
                            nextStage->launch();
                            break;
                        }
                    }
                }
            }
        }
    }
    */
    updateStatus();
}



// Resets flow
void Flow::reset()
{
    foreach(Stage* stage, _stages) {
        stage->reset();
    }
    updateStatus();
    updateResult();
}



// Returns flow result
Flow::Result Flow::result()
{
    return _result;
}


void Flow::setBaseRoutesList(const QStringList &baseRoutes)
{
    if (_baseRoutes != baseRoutes) {
        _baseRoutes = baseRoutes;
        onBaseRoutesListChanged(baseRoutes);
    }
}


// Sets flow directory path
void Flow::setDirPath(const QString &dirPath)
{
    if (_dirPath != dirPath) {
        _dirPath = dirPath;
        onDirPathChanged(dirPath);
    }
}



// Sets flow result
void Flow::setResult(Result result)
{
    if (_result != result) {
        _result = result;
        emit resultChanged(_result);
    }
}



// Sets flow status
void Flow::setStatus(Status status)
{
    if (_status != status) {

        /*
        switch (status) {
        case Status_UserSkipped:
            qDebug() << "flow status = Status_UserSkipped";
            break;
        case Status_Pending:
            qDebug() << "flow status = Status_Pending";
            break;
        case Status_Running:
            qDebug() << "flow status = Status_Running";
            break;
        case Status_AutoSkipped:
            qDebug() << "flow status = Status_AutoSkipped";
            break;
        case Status_Complited:
            qDebug() << "flow status = Status_Complited";
            break;
        case Status_Error:
            qDebug() << "flow status = Status_Error";
            break;
        case Status_Unknown:
            qDebug() << "flow status = Status_Unknown";
            break;
        default:
            qDebug() << "flow status = ???";
            break;
        }
        */

        _status = status;
        emit statusChanged(status);
    }
}



// Returns stages of the flow
QList<Stage*> Flow::stages()
{
    return _stages;
}



// Returns flow status
Flow::Status Flow::status()
{
    return _status;
}



// Updates flow result
void Flow::updateResult()
{
    bool unknown = 0;
    bool ok = 0;
    bool warning = 0;
    bool error = 0;

    foreach (Stage* stage, _stages) {
        Stage::Result stageResult = stage->result();
        if (stageResult == Stage::Result_Unknown) {
            unknown = 1;
        } else if (stageResult == Stage::Result_Ok) {
            ok = 1;
        } else if (stageResult == Stage::Result_Warning) {
            warning = 1;
        } else if (stageResult == Stage::Result_Error) {
            error = 1;
        }
    }

    if (error) {
        setResult(Result_Error);
    } else if (warning) {
        setResult(Result_Warning);
    } else if (ok) {
        setResult(Result_Ok);
    } else {
        setResult(Result_Unknown);
    }

    Q_UNUSED(unknown);
}



// Updates flow status
void Flow::updateStatus()
{
    bool userSkipped = false;
    bool pending = false;
    bool running = false;
    bool autoSkipped = false;
    bool complited = false;
    bool error = false;

    Stage::Status stageStatus;
    foreach (Stage* stage, _stages) {
        stageStatus = stage->status();
        if (stageStatus == Stage::Status_UserSkipped) {
            userSkipped = true;
        } else if (stageStatus == Stage::Status_Pending) {
            pending = true;
        } else if (stageStatus == Stage::Status_Running) {
            running = true;
        } else if (stageStatus == Stage::Status_AutoSkipped) {
            autoSkipped = true;
        }  else if (stageStatus == Stage::Status_Complited) {
            complited = true;
        } else if (stageStatus == Stage::Status_Error) {
            error = true;
        }
    }

    if (userSkipped && !pending && !running && !autoSkipped && !complited && !error) {
        setStatus(Status_UserSkipped);
    } else if (pending && !running && !complited && !error) {
        setStatus(Status_Pending);
    } else if (running) {
        setStatus(Status_Running);
    } else if (!pending && !running && autoSkipped && !complited && !error) {
        setStatus(Status_AutoSkipped);
    } else if (!running && complited && !error) {
        setStatus(Status_Complited);
    } else if (!running && error) {
        setStatus(Status_Error);
    } else {
        setStatus(Status_Unknown);
    }
}
