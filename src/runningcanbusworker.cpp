#include "runningcanbusworker.h"

runningCanBusWorker::runningCanBusWorker(CanBusWorkerBasis *parentBasis, QState *parent) :
    QState(parent),
    basisPtr(parentBasis)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("runningCanBusWorker Constructed"));
}

void runningCanBusWorker::onEntry(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("runningCanBusWorker Entered"));
    qApp->processEvents();
    basisPtr->executePrioritizedBuffer();
    if (basisPtr->prioritizedBuffer.size())
    {
        emit basisPtr->requestDirectTransition("runningCanBusWorker");
    }
    else
    {
        emit basisPtr->requestDirectTransition("idleCanBusWorker");
    }
}

void runningCanBusWorker::onExit(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("Leave runningCanBusWorker"));
    basisPtr->previousStateName = objectName();
}
