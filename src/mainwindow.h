#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QMainWindow>
#include <QSignalMapper>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include "flow.h"
#include "outputdialog1.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Flow* _flow;
    OutputDialog1* _outputDialog;
    QSignalMapper _showConfigMapper;
    QButtonGroup _settingsBg;
    Ui::MainWindow *ui;
    QSignalMapper _updateSettingsButtonMapper;

private slots:
    void addBaseRoute(const QString &name = QString());
    void browseFlowDir();
    void checkStages();
    void connectToDatabase(bool requestCredentials = false);
    void forceStages();
    void launch();
    void loadLastConfiguration();
    void onFlowStatusChanged(Flow::Status status);
    void onOutputDialogClosed();
    void removeBaseRoute();
    void setBaseRoutes(const QStringList &baseRoutes);
    void showConfig(QWidget* w);
    void showManual();
    void updateBaseRoutes();
    void updateFlowDirPathCmb();
    void updateCheckAllCmb();
    void updateForceAllCmb();
    void updateSettingsButton(QWidget* w);
};

#endif // MAINWINDOW_H
