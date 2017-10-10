#include "idlecanbusworker.h"

idleCanBusWorker::idleCanBusWorker(CanBusWorkerBasis *parentBasis, QState *parent) :
    QState(parent),
    basisptr(parentBasis)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("idleCanBusWorker Constructed"));
}

void idleCanBusWorker::onEntry(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("idleCanBusWorker Entered"));
    qApp->processEvents();
}

void idleCanBusWorker::onExit(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("Leave idleCanBusWorker"));
    basisptr->previousStateName = objectName();
}
