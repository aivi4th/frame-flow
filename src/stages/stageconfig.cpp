#include <QDebug>
#include "stageconfig.h"



// Constructor 1
StageConfig::StageConfig(QWidget *parent) :
    QWidget(parent)
{
    updateStatus();
}



// Destructor
StageConfig::~StageConfig()
{

}



// Retrurns config keys from config content string
QStringList StageConfig::configKeys(const QString &configContent)
{
    QStringList keys;
    QString group;
    foreach (QString line, configContent.split('\n')) {
        if (QRegExp("^\\s*\\[.*\\]\\s*$").exactMatch(line)) {
            line.remove(QRegExp("[\\[\\]]"));
            group = line.trimmed();
        } else if (QRegExp("^\\s*\\S+\\s*=.*$").exactMatch(line)) {
            if (group.isEmpty()) {
                keys << line.split('=')[0].trimmed();
            } else {
                keys << QString(group).append('/').append(line.split('=')[0].trimmed());
            }
        }
    }
    return keys;
}



// Retrurns value of config key from config content string
QVariant StageConfig::configValue(const QString &configContent, const QString &configKey)
{
    QVariant value;

    QString group;
    QString key;
    if (QRegExp("^\\S+/\\S+$").exactMatch(configKey)) {
        group = configKey.split('/')[0];
        key = configKey.split('/')[1];
    } else {
        key = configKey;
    }


    QString currentGroup;
    foreach (QString line, configContent.split('\n')) {
        if (QRegExp("^\\s*\\[.*\\]\\s*$").exactMatch(line)) {
            line.remove(QRegExp("[\\[\\]]"));
            currentGroup = line.trimmed();
        }

        if (currentGroup == group && QRegExp(QString("^\\s*").append(key).append("\\s*=\\s*\\S.*$")).exactMatch(line)) {
            value = line.split('=')[1].trimmed().split(QRegExp("\\s*,\\s*"));
            break;
        }
    }

    return value;
}


void StageConfig::onBaseRoutesListChanged(const QStringList &baseRoutes)
{
    Q_UNUSED(baseRoutes);
}


// Do, when flow directory path is changed
void StageConfig::onFlowDirPathChanged(const QString &dirPath)
{
    Q_UNUSED(dirPath);
}


void StageConfig::setArgs(const QString &args)
{
    Q_UNUSED(args);
}


void StageConfig::setBaseRoutesList(const QStringList &baseRoutes)
{
    if (_baseRoutes != baseRoutes) {
        _baseRoutes = baseRoutes;
        onBaseRoutesListChanged(baseRoutes);
    }
}


// Sets flow directory path to "dirPath"
void StageConfig::setFlowDirPath(const QString &dirPath)
{
    if (_flowDirPath != dirPath) {
        _flowDirPath = dirPath;
        onFlowDirPathChanged(dirPath);
    }
}



// Sets stage config status to "status"
void StageConfig::setStatus(Status status)
{
    if (_status != status) {
        _status = status;
        emit statusChanged(status);
    }
}



// Returns stage config status
StageConfig::Status StageConfig::status()
{
    return _status;
}



// Updates stage config status
void StageConfig::updateStatus()
{
    setStatus(Status_Ok);
}


