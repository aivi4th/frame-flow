#ifndef WORKDIR_CONFIG_H
#define WORKDIR_CONFIG_H

#include "stageconfig.h"

namespace Ui {
    class WorkDir_config;
}

class WorkDir_config : public StageConfig
{
    Q_OBJECT
    
public:
    explicit WorkDir_config(const QString &title, QWidget *parent = 0);
    ~WorkDir_config();

public slots:
    QString baseFramePath();
    void setArgs(const QString &args = QString());
    
private:
    Ui::WorkDir_config *ui;

private slots:
    void browseBaseFrame();
    void onBaseFramePathChanged(const QString &path);
    void updateStatus();
};

#endif // WORKDIR_CONFIG_H
