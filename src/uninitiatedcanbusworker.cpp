#include "uninitiatedcanbusworker.h"

uninitiatedCanBusWorker::uninitiatedCanBusWorker(CanBusWorkerBasis *parentBasis) :
    basisptr(parentBasis)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("uninitiatedCanBusWorker Constructed"));
}

void uninitiatedCanBusWorker::onEntry(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("uninitiatedCanBusWorker Entered"));
    if (basisptr->previousStateName.isEmpty() && !(basisptr->currentDev))
    {
        basisptr->initialize();
    }
}

void uninitiatedCanBusWorker::onExit(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("Leave uninitiatedCanBusWorker"));
}
