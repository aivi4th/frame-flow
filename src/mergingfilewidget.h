#ifndef MERGINGFILEWIDGET_H
#define MERGINGFILEWIDGET_H

#include <QFutureWatcher>
#include <QMap>
#include <QWidget>

namespace Ui {
class MergingFileWidget;
}

class MergingFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MergingFileWidget(QWidget *parent = nullptr);
    explicit MergingFileWidget(const QString &text, const QStringList &baseRoutes, const QString &currentConfig, const QString &dataConfig, QWidget *parent = nullptr);
    ~MergingFileWidget();

public slots:
    bool isChecked();
    void setChecked(bool checked = true);
    void setCurrentText(const QString &text = QString());
    QString text();
    QString currentText();

signals:
    void toggled(bool);
    void editTextChanged(const QString &text);
    void updateStatus();

private:
    QStringList _baseRoutes;
    QString _currentConfig;
    QString _dataConfig;
    QFuture<QStringList> _getMergingFilesFuture;
    QFutureWatcher<QStringList> _getMergingFilesWatcher;
    Ui::MergingFileWidget *ui;

private slots:
    void browseMergingFile();
    bool eventFilter(QObject *watched, QEvent *event);
    QStringList getMergingFiles(const QMap<QString,QString> &technologyToBaseRoute);
    void initMergingFilesUpdate();
    void onMergingFileChanged(const QString &mergingFile);
    bool svnFileExists(const QString &svnFilePath);
    void updateMergingFiles();
    void updateMergingFile();

};

#endif // MERGINGFILEWIDGET_H
