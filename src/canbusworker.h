#ifndef CANBUSWORKER_H
#define CANBUSWORKER_H

#define CanBusWorkerDbgEn 1

#include <QStateMachine>
#include "canbusworkerbasis.h"

class CanBusWorker : public QStateMachine
{
    Q_OBJECT
public:
    explicit CanBusWorker(QObject *parent = 0);
    ~CanBusWorker();
signals:
    void Out(const GlobalSignal &);
public slots:
    void In(const GlobalSignal &aGlobalSignal);
private:
    CanBusWorkerBasis * currentBasis = Q_NULLPTR;
};

#endif // CANBUSWORKER_H
