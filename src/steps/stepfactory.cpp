#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QSettings>
#include "calibrestep.h"
#include "dataconfstep.h"
#include "stepfactory.h"



// Creates step
Step* StepFactory::createStep(const QString &id, const QMap<QString, QString> &args, QWidget* parent)
{
    QSettings commonConfig(QFileInfo(QApplication::applicationDirPath()).dir().path().append("/cfg/common.conf"), QSettings::IniFormat);
    QString tclInterpretator = commonConfig.value("tclsh").toString();
    QString calInterpretator = commonConfig.value("cadToolsDir").toString().append("/mentor/").append(commonConfig.value("Calibre/defaultVersion").toString()).append("/bin/calibrewb");

    QString name;
    QString interpretator;
    QString script;
    if (id == "pcg") {
        name = "Подготовка структуры рабочего каталога";
        interpretator = tclInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/pcg.tcl");
        return new ScriptStep(id, name, interpretator, script, args, parent);
    } else if (id == "dataconf") {
        name = "Создание файла конфигурации";
        return new DataConfStep(id, name, args, parent);
    } else if (id == "frame2cells") {
        name = "Извлечение ячеек из базового фрейма";
        interpretator = calInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/frame2cells.tcl");
        return new CalibreStep(id, name, interpretator, script, args, parent);
    } else if (id == "cadarray") {
        name = "Формирование массива \"" + args["array"] + "\" в топологических слоях";
        interpretator = calInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/cadarray.tcl");
        return new CalibreStep(id, name, interpretator, script, args, parent);
    } else if (id == "maskarray") {
        name = "Формирование массива \"" + args["array"] + "\" в масочных слоях";
        interpretator = calInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/maskarray.tcl");
        return new CalibreStep(id, name, interpretator, script, args, parent);
    } else if (id == "cadplt") {
        name = "Формирование заглушки \"" + args["plt"] + "\" в топологических слоях";
        interpretator = calInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/cadplt.tcl");
        return new CalibreStep(id, name, interpretator, script, args, parent);
    } else if (id == "maskplt") {
        name = "Формирование заглушки \"" + args["plt"] + "\" в масочных слоях";
        interpretator = calInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/maskplt.tcl");
        return new CalibreStep(id, name, interpretator, script, args, parent);
    } else if (id == "cadframe") {
        name = "Формирование фрейма \"" + args["libName"] + "\" в топологических слоях";
        interpretator = calInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/cadframe.tcl");
        return new CalibreStep(id, name, interpretator, script, args, parent);
    } else if (id == "maskframe") {
        name = "Формирование фрейма \"" + args["libName"] + "\" в масочных слоях";
        interpretator = calInterpretator;
        script = QFileInfo(QApplication::applicationDirPath()).dir().path().append("/scr/steps/maskframe.tcl");
        return new CalibreStep(id, name, interpretator, script, args, parent);
    } else {
        return 0;
    }
    return 0;
}
