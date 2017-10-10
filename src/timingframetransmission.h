#ifndef TIMINGFRAMETRANSMISSION_H
#define TIMINGFRAMETRANSMISSION_H

#include "canbusworkerbasis.h"
#include <QTimer>

class timingFrameTransmission : public QState
{
public:
    timingFrameTransmission(CanBusWorkerBasis *parentBasis, QState *parent = nullptr);
protected:
    void onEntry(QEvent *) override;
    void onExit(QEvent *) override;
private:
    CanBusWorkerBasis * basisPtr = nullptr;
    QTimer timer;
};

#endif // TIMINGFRAMETRANSMISSION_H
