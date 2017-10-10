#include "canbusworker.h"

CanBusWorker::CanBusWorker(QObject *parent) : QStateMachine(parent)
{    
    currentBasis = new CanBusWorkerDB(this);
    QObject::connect(currentBasis, &CanBusWorkerBasis::Out, this, &CanBusWorker::Out);

    QState *main = new QState();
    main->setObjectName("main");
    uninitiatedCanBusWorker *state0 = new uninitiatedCanBusWorker(currentBasis,main);
    state0->setObjectName("uninitiatedCanBusWorker");
    idleCanBusWorker *state1 = new idleCanBusWorker(currentBasis,main);
    state1->setObjectName("idleCanBusWorker");
    runningCanBusWorker *state2 = new runningCanBusWorker(currentBasis,main);
    state2->setObjectName("runningCanBusWorker");
    timingFrameTransmission *state3 = new timingFrameTransmission(currentBasis,main);
    state3->setObjectName("timingFrameTransmission");

    state0->addTransition(currentBasis, &CanBusWorkerBasis::isInitialized, state1);
    state0->addTransition(currentBasis, &CanBusWorkerBasis::aGlobalSignalAdded, state2);
    state1->addTransition(currentBasis, &CanBusWorkerBasis::aGlobalSignalAdded, state2);
    state2->addTransition(new directTransition(currentBasis,SIGNAL(requestDirectTransition(QString)),state1);
    state2->addTransition(new directTransition(currentBasis,SIGNAL(requestDirectTransition(QString)),state2);
    state2->addTransition(currentBasis, &CanBusWorkerBasis::writingFrame, state3);
    state3->addTransition(new FrameIsWritten(currentBasis,state2));

    main->setInitialState(state0);

    errorCanBusWorker *state7 = new errorCanBusWorker(currentBasis);
    state7->setObjectName("errorCanBusWorker");

    main->addTransition(currentBasis, &CanBusWorkerBasis::ErrorOccurred, state7);

    addState(main);
    addState(state7);
    setInitialState(main);
    anIf(CanBusWorkerDbgEn, anTrk("CanBusWorker Constructed"));
}

void CanBusWorker::In(const GlobalSignal &aGlobalSignal)
{    
    if (isRunning())
    {
        currentBasis->In(aGlobalSignal);
    }
}
