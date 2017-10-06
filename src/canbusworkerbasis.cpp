#include "canbusworkerbasis.h"

CanBusWorkerBasis::CanBusWorkerBasis(QObject *parent) : AbstractStateMachineBasis(parent)
{
    registerGlobalSignal;
    qRegisterMetaType<QCanBusFrame>("QCanBusFrame");
    qRegisterMetaType<QCanBusDevice::CanBusDeviceState>("QCanBusDevice::CanBusDeviceState");
    qRegisterMetaType<QCanBusDevice::CanBusError>("QCanBusDevice::CanBusError");
    anIf(CanBusWorkerBasisDbgEn, anTrk("CanBusWorkerBasis Constructed !"));
}

CanBusWorkerBasis::~CanBusWorkerBasis()
{
    dispose();
    anIf(CanBusWorkerBasisDbgEn, anTrk("CanBusWorkerBasis Destroyed !"));
}

void CanBusWorkerBasis::initialize()
{
    dispose();
    QString * DeviceCreationErrorString = nullptr;
#if QT_VERSION >= QT_VERSION_CHECK(5,8,0)
    currentDev = QCanBus::instance()->createDevice(QStringLiteral("socketcan"),
                                                          QStringLiteral("can0"),
                                                          DeviceCreationErrorString);
#else
    currentDev = QCanBus::instance()->createDevice(QByteArray("socketcan"),
                                                          QStringLiteral("can0"));
#endif
    if (currentDev)
    {
        anIf(CanBusWorkerBasisDbgEn,
            anTrk("Device Created !");
            QObject::connect(currentDev, &QCanBusDevice::stateChanged, this, [&](QCanBusDevice::CanBusDeviceState devState){
                anAck("Device State Changed To " + QCanBusDeviceStateMetaEnum.valueToKey(static_cast<int>(devState)));
            });)
        QObject::connect(currentDev, &QCanBusDevice::errorOccurred, this, [&](QCanBusDevice::CanBusError devError){
            setError(DeviceError,QCanBusErrorMetaEnum.valueToKey(static_cast<int>(devError)));
        });
        QObject::connect(currentDev, &QCanBusDevice::framesReceived, this, &CanBusWorkerBasis::FramesReceived,
                         uniqueQtConnectionType);
        QObject::connect(currentDev, &QCanBusDevice::framesWritten, this, &CanBusWorkerBasis::FramesWritten,
                         uniqueQtConnectionType);
        if (currentDev->connectDevice())
        {
            anIf(CanBusWorkerBasisDbgEn, anAck("Device Connected !"));
            GlobalSignal notifyDeviceCreated;
            notifyDeviceCreated.Type = QVariant::fromValue(DeviceReady);
            notifyDeviceCreated.TimeStamp = NOW2String;
            notifyDeviceCreated.DstStrs.append(SmallCoordinatorObjName);
            emit isInitialized();
            emit Out(notifyDeviceCreated);
        }
        else
        {
            anIf(CanBusWorkerBasisDbgEn, anError("Failed To Connect Device !"));
        }
    }
    else
    {
        setError(DeviceCreationFailed,DeviceCreationErrorString?*DeviceCreationErrorString:QStringLiteral());
    }
    delete DeviceCreationErrorString;
    DeviceCreationErrorString = nullptr;
    anIf(CanBusWorkerBasisDbgEn, anTrk("CanBusWorkerBasis Initialized !"));
}

void CanBusWorkerBasis::dispose()
{
    anIf(CanBusWorkerBasisDbgEn && currentDev, anTrk("Clean CanBusWorkerBasis !"));
    previousStateName.clear();
    clearPrioritizedBuffer();
    clearError();
    if (currentDev)
    {
        currentDev->disconnectDevice();
        delete currentDev;
        currentDev = nullptr;
    }
}

void CanBusWorkerBasis::clearCache()
{
    anIf(CanBusWorkerBasisDbgEn, anTrk("Clear Cache !"));
    currentGlobalSignal.Data.clear();
    currentGlobalSignal.Type.clear();
    currentGlobalSignal.TimeStamp.clear();
    currentGlobalSignal.DstStrs.clear();
    currentGlobalSignal.Key.clear();
    currentGlobalSignal.Priority = 0;
    currentGlobalSignal.SignalPriority = 0;
    lastFrameTransmitted.setFrameId(0);
    lastFrameTransmitted.setPayload(0);
}

void CanBusWorkerBasis::setError(const CanBusWorkerBasis::Error &anErrorType, const QString &anErrorInfo)
{
    if (anErrorType!=NoError)
    {
        anIf(CanBusWorkerBasisDbgEn, anError("Error Occurred !"));
        ErrorType = anErrorType;
        ErrorInfo = anErrorInfo;
        emit ErrorOccurred();
    }
}

void CanBusWorkerBasis::clearError()
{
    anIf(CanBusWorkerBasisDbgEn && (ErrorType!=NoError), anInfo("Clear Error !"));
    ErrorType = NoError;
    ErrorInfo.clear();
}

void CanBusWorkerBasis::executePrioritizedBuffer()
{
    clearCache();
    if (prioritizedBuffer.size())
    {
        currentGlobalSignal = takeOutFirstOfMostPrioritizedGlobalSignals();
        if (currentGlobalSignal->Type.typeName() == QStringLiteral("CanBusWorkerBasis::Data"))
        {
            switch (currentGlobalSignal->Type.toInt()) {
            case FrameReceived:
            {
                GlobalSignal pendingReplyFrameWithTimeStamp;
                pendingReplyFrameWithTimeStamp.Type = QVariant::fromValue(replyFrameWithTimeStamp);
                pendingReplyFrameWithTimeStamp.DstStrs.append(SmallCoordinatorObjName);
                while (currentDev->framesAvailable())
                {
                    pendingReplyFrameWithTimeStamp.Data = QVariant::fromValue(currentDev->readFrame());
                    pendingReplyFrameWithTimeStamp.TimeStamp = NOW2String;
                    addAGlobalSignal(pendingReplyFrameWithTimeStamp);
                }
                break;
            }
            case replyFrameWithTimeStamp:
            {
                emit Out(currentGlobalSignal);
                break;
            }
            case requestFrameTransmission:
            {
                lastFrameTransmitted = currentGlobalSignal;
                break;
            }
            default:
                break;
            }
        }
    }
}

const QMetaEnum CanBusWorkerBasis::DataMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Data>();
const QMetaEnum CanBusWorkerBasis::ErrorMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Error>();
const QMetaEnum CanBusWorkerBasis::WarningMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Warning>();
const QMetaEnum CanBusWorkerBasis::NotificationMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Notification>();
const QMetaEnum CanBusWorkerBasis::QCanBusErrorMetaEnum = QMetaEnum::fromType<QCanBusDevice::CanBusError>();
const QMetaEnum CanBusWorkerBasis::QCanBusDeviceStateMetaEnum = QMetaEnum::fromType<QCanBusDevice::CanBusDeviceState>();
