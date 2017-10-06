#ifndef RUNNINGCANBUSWORKER_H
#define RUNNINGCANBUSWORKER_H

#include "canbusworkerbasis.h"

class runningCanBusWorker : public QState
{
public:
    runningCanBusWorker(CanBusWorkerBasis *parentBasis);
protected:
    void onEntry(QEvent *) override;
    void onExit(QEvent *) override;
private:
    CanBusWorkerBasis * basisPtr = nullptr;
};

#endif // RUNNINGCANBUSWORKER_H
