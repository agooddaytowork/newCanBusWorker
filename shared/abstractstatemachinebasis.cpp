#include "abstractstatemachinebasis.h"

AbstractStateMachineBasis::AbstractStateMachineBasis(QObject *parent) : QObject(parent)
{

}

void AbstractStateMachineBasis::addAGlobalSignal(const GlobalSignal &aGlobalSignal)
{
    if (prioritizedBuffer.contains(aGlobalSignal.Priority))
    {
        prioritizedBuffer[aGlobalSignal.Priority].append(aGlobalSignal);
    }
    else
    {
        prioritizedBuffer.insert(aGlobalSignal.Priority, QList<GlobalSignal>({aGlobalSignal}));
    }
    deleteEmptyListsFromPrioritizedBuffer();
    if ((prioritizedBuffer.size() == 1) && (prioritizedBuffer.first().size() == 1))
    {
        emit prioritizedBufferRefilled();
    }
    emit aGlobalSignalAdded();
}

GlobalSignal &AbstractStateMachineBasis::takeOutFirstOfMostPrioritizedGlobalSignals()
{
    GlobalSignal &tmpReturn = prioritizedBuffer.last().first();
    prioritizedBuffer.last().removeFirst();
    deleteEmptyListsFromPrioritizedBuffer();
    return tmpReturn;
}

GlobalSignal &AbstractStateMachineBasis::takeOutLastOfMostPrioritizedGlobalSignals()
{
    GlobalSignal &tmpReturn = prioritizedBuffer.last().last();
    prioritizedBuffer.last().removeLast();
    deleteEmptyListsFromPrioritizedBuffer();
    return tmpReturn;
}

void AbstractStateMachineBasis::deleteEmptyListsFromPrioritizedBuffer()
{
    while (prioritizedBuffer.size())
    {
        if (prioritizedBuffer.last().size())
            break;
        else
        {
            prioritizedBuffer.remove(prioritizedBuffer.lastKey());
        }
    }
}

void AbstractStateMachineBasis::clearPrioritizedBuffer()
{
    if (prioritizedBuffer.size())
        prioritizedBuffer.clear();
}
