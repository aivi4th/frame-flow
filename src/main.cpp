#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    // Create application
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setOrganizationName("MERI");
    app.setOrganizationDomain("niime.ru");
    app.setApplicationName("frame-flow");
    app.setApplicationVersion(QString("%1.%2.%3.%4").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_MICRO).arg(VERSION_BUILD));
    app.setProperty("description", "Программный комплекс для проектирования технологического обрамления (FRAME)");
    app.setProperty("commonConfigPath", QFileInfo(QCoreApplication::instance()->applicationDirPath()).dir().path() + "/cfg/common.conf");

    // Create main window
    MainWindow* mainWindow = new MainWindow;
    mainWindow->setWindowTitle(app.applicationName().append(" ").append(app.applicationVersion()));
    int x = QApplication::desktop()->availableGeometry(QCursor::pos()).center().x();
    int y = QApplication::desktop()->availableGeometry(QCursor::pos()).center().y();
    x-= mainWindow->width()/2;
    y-= mainWindow->height()/2;
    mainWindow->move(QPoint(x,y));
    mainWindow->show();

    return app.exec();
}
