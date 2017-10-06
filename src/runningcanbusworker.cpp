#include "runningcanbusworker.h"

runningCanBusWorker::runningCanBusWorker(CanBusWorkerBasis *parentBasis) :
    basisPtr(parentBasis)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("runningCanBusWorker Constructed"));
}

void runningCanBusWorker::onEntry(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("runningCanBusWorker Entered"));
}

void runningCanBusWorker::onExit(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("Leave runningCanBusWorker"));
}
