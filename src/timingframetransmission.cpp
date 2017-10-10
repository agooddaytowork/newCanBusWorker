#include "timingframetransmission.h"

timingFrameTransmission::timingFrameTransmission(CanBusWorkerBasis *parentBasis, QState *parent) :
    QState(parent)
{
    timer.setParent(this);
    timer.setInterval(3000);
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, this, [parentBasis](){
        parentBasis->setError(CanBusWorkerBasis::FrameWrittenTimedOut,QStringLiteral(""));
    });
    anIf(CanBusWorkerBasisDbgEn, anTrk("timingFrameTransmission Constructed"));
}

void timingFrameTransmission::onEntry(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("timingFrameTransmission Entered"));
    timer.start();
    qApp->processEvents();
}

void timingFrameTransmission::onExit(QEvent *)
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("Leave timingFrameTransmission"));
    timer.stop();
}
