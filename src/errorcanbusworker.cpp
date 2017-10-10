#include "errorcanbusworker.h"

errorCanBusWorker::errorCanBusWorker(CanBusWorkerBasis *parentBasis, QState *parent) :
    QState(parent),
    basisptr(parentBasis)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("errorCanBusWorker Constructed"));
}

void errorCanBusWorker::onEntry(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("errorCanBusWorker Entered"));
    qApp->processEvents();
    basisptr->emitErrorGlobalSignal();
}

void errorCanBusWorker::onExit(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("Leave errorCanBusWorker"));
}
