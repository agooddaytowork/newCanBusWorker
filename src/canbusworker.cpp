#include "canbusworker.h"

CanBusWorker::CanBusWorker(QObject *parent) : QStateMachine(parent)
{
    anIf(CanBusWorkerDbgEn, anTrk("Object Constructed !"));
    currentBasis = new CanBusWorkerDB(this);

    waitForPluginAndInterface * state0 = new waitForPluginAndInterface(currentBasis,1000);
    state0->setObjectName(QStringLiteral("waitForPluginAndInterface"));
    createDevice * state1 = new createDevice(currentBasis);
    state1->setObjectName(QStringLiteral("createDevice"));
    connectDevice * state2 = new connectDevice(currentBasis);
    state2->setObjectName(QStringLiteral("connectDevice"));
    readFrame * state3 = new readFrame(currentBasis);
    state3->setObjectName(QStringLiteral("readFrame"));

    writeFrame * state4 = new writeFrame();
    state4->setObjectName(QStringLiteral("writeFrame"));
    writeAFrame * state4SubStateWriteAFrame = new writeAFrame(state4,currentBasis);
    QState * state4SubStateDone = new QState(state4);
    FrameSent * signalFrameSent = new FrameSent(currentBasis);
    signalFrameSent->setTargetState(state4SubStateDone);
    state4SubStateWriteAFrame->addTransition(signalFrameSent);
    state4->setInitialState(state4SubStateWriteAFrame);

    waitForErrorHandler * state7 = new waitForErrorHandler(currentBasis,1000);
    state7->setObjectName(QStringLiteral("waitForErrorHandler"));

    DeviceConnected * state2Tostate3 = new DeviceConnected(currentBasis);
    state2Tostate3->setTargetState(state3);

    state0->addTransition(currentBasis, &CanBusWorkerDB::PluginAndInterfaceChanged, state1);
    state1->addTransition(new directTransition(currentBasis,SIGNAL(directTransitionRequest(QString)),state2));
    state1->addTransition(currentBasis, &CanBusWorkerDB::ErrorOccurred, state7);
    state2->addTransition(currentBasis, &CanBusWorkerDB::PluginAndInterfaceChanged, state1);
    state2->addTransition(currentBasis, &CanBusWorkerDB::ErrorOccurred, state7);
    state2->addTransition(state2Tostate3);
    state3->addTransition(currentBasis, &CanBusWorkerDB::PluginAndInterfaceChanged, state1);
    state3->addTransition(currentBasis, &CanBusWorkerDB::ErrorOccurred, state7);
    state3->addTransition(currentBasis, &CanBusWorkerDB::aFrameAdded, state4);
    state3->addTransition(currentBasis, &CanBusWorkerDB::FrameReceived, state3);
    state4->addTransition(currentBasis, &CanBusWorkerDB::ErrorOccurred, state7);
    state4SubStateDone->addTransition(currentBasis, &CanBusWorkerDB::PluginAndInterfaceChanged, state1);
    state4SubStateDone->addTransition(currentBasis, &CanBusWorkerDB::aFrameAdded, state4);
    state4SubStateDone->addTransition(currentBasis, &CanBusWorkerDB::FrameReceived, state3);
    state7->addTransition(currentBasis, &CanBusWorkerDB::freshRetry, state0);
    state7->addTransition(currentBasis, &CanBusWorkerDB::PluginAndInterfaceChanged, state1);
    state7->addTransition(currentBasis, &CanBusWorkerDB::reconnectDevice, state2);
    state7->addTransition(currentBasis, &CanBusWorkerDB::retry, state3);

    this->addState(state0);
    this->addState(state1);
    this->addState(state2);
    this->addState(state3);
    this->addState(state4);
    this->addState(state7);
    this->setInitialState(state0);
    this->setErrorState(state7);

    QObject::connect(currentBasis, &CanBusWorkerDB::Out, this, &CanBusWorker::Out,
                     CanBusWorkerDB::uniqueQtConnectionType);
}

CanBusWorker::~CanBusWorker()
{
    if (currentBasis)
    {
        delete currentBasis;
        currentBasis = Q_NULLPTR;
    }
    anIf(CanBusWorkerDbgEn, anTrk("CanBusWorker Destroyed !"));
}

void CanBusWorker::In(const GlobalSignal &aGlobalSignal)
{    
    if (this->isRunning())
    {
        anIf(CanBusWorkerDbgEn, anTrk("Signal-To-CanBusWorker Received !"));
        QString enumVarTypeName(aGlobalSignal.typeName());
        if (enumVarTypeName == QStringLiteral("CanBusWorkerDB::Data"))
        {
            switch (aGlobalSignal.toInt()) {
            case CanBusWorkerDB::requestPluginAndInterface:
            {
                anIf(CanBusWorkerDbgEn, anInfo("requestPluginAndInterface"));
                emit Out(QVariant::fromValue(CanBusWorkerDB::requestPluginAndInterface),
                         QVariant::fromValue(currentBasis->currentPluginAndInterface));
                break;
            }
            case CanBusWorkerDB::replyPluginAndInterface:
            {
                anIf(CanBusWorkerDbgEn, anInfo("replyPluginAndInterface"));
                currentBasis->currentPluginAndInterface = dataVar.value<CanBusWorkerDB::PluginNameAndInterfaceName>();
                emit currentBasis->PluginAndInterfaceChanged();
                break;
            }
            case CanBusWorkerDB::clearPendingFrameList:
            {
                anIf(CanBusWorkerDbgEn, anInfo("clearPendingFrameList"));
                currentBasis->pendingFrameList.clear();
                break;
            }
            case CanBusWorkerDB::addAFrameIntoPendingFrameList:
            {
                anIf(CanBusWorkerDbgEn, anInfo("addAFrameIntoPendingFrameList"));
                currentBasis->pendingFrameList.append(dataVar.value<QCanBusFrame>());
                emit currentBasis->aFrameAdded();
                break;
            }
            default:
                break;
            }
        }
    }
}
