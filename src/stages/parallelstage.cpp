#include <QDebug>
#include "parallelstage.h"



// Construct 1
ParallelStage::ParallelStage(const QString &id, const QString &name, const QString &flowDirPath, const QMap<QString, QString> &args, QWidget *parent) :
    Stage(id, name, flowDirPath, args, parent)
{

}



// Destructor
ParallelStage::~ParallelStage()
{

}



// Launches stage
void ParallelStage::launch()
{
    foreach (Operation* operation, _operations) {
        if (operation->status() == Operation::Status_Pending) {
            operation->launch();
        }
    }
}
