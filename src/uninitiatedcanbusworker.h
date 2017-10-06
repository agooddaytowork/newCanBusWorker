#ifndef UNINITIATEDCANBUSWORKER_H
#define UNINITIATEDCANBUSWORKER_H

#include "canbusworkerbasis.h"

class uninitiatedCanBusWorker : public QState
{
public:
    uninitiatedCanBusWorker(CanBusWorkerBasis *parentBasis);
protected:
    void onEntry(QEvent *) override;
    void onExit(QEvent *) override;
private:
    CanBusWorkerBasis * basisptr = nullptr;
};

#endif // UNINITIATEDCANBUSWORKER_H
