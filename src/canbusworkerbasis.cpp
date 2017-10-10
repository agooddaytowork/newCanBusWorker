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
                anAck("Device State Changed To " << QCanBusDeviceStateMetaEnum.valueToKey(static_cast<int>(devState)));
            });)
        QObject::connect(currentDev, &QCanBusDevice::errorOccurred, this, [&](QCanBusDevice::CanBusError devError){
            setError(DeviceError,QCanBusErrorMetaEnum.valueToKey(static_cast<int>(devError)));
        });
        QObject::connect(currentDev, &QCanBusDevice::framesReceived, this, [&](){
            GlobalSignal queueFramesRead;
            queueFramesRead.Type = QVariant::fromValue(FrameReceived);
            addAGlobalSignal(queueFramesRead);
        });
        QObject::connect(currentDev, &QCanBusDevice::framesWritten, this, &CanBusWorkerBasis::FramesWritten,
                         uniqueQtConnectionType);
        if (currentDev->connectDevice())
        {
            anIf(CanBusWorkerBasisDbgEn, anAck("Device Connected !"));
            GlobalSignal notifyDeviceCreated;
            notifyDeviceCreated.Type = QVariant::fromValue(DeviceReady);
            notifyDeviceCreated.TimeStamp = NOW2String;
            notifyDeviceCreated.DstStrs.append(SmallCoordinatorObjName);
            addAGlobalSignal(notifyDeviceCreated);
            emit isInitialized();
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
    currentGlobalSignal = GlobalSignal();
    lastFrameWritten.setFrameId(0);
    lastFrameWritten.setPayload("");
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
        QString currentGlobalSignalTypeTypeName = currentGlobalSignal.Type.typeName();
        if (currentGlobalSignalTypeTypeName == QStringLiteral("CanBusWorkerBasis::Data"))
        {
            switch (currentGlobalSignal.Type.toInt()) {
            case replyFrameWithTimeStamp:
            {
                emit Out(currentGlobalSignal);
                break;
            }
            case requestFrameTransmission:
            {
                lastFrameWritten = currentGlobalSignal.Data.value<QCanBusFrame>();
                currentDev->writeFrame(lastFrameWritten);
                emit writingFrame();
                break;
            }
            case clearBuffer:
            {
                clearPrioritizedBuffer();
                break;
            }
            default:
                break;
            }
        }
        else if (currentGlobalSignalTypeTypeName == QStringLiteral("CanBusWorkerBasis::Notification"))
        {
            switch (currentGlobalSignal.Type.toInt()) {
            case DeviceReady:
            {
                emit Out(currentGlobalSignal);
                break;
            }
            case FrameReceived:
            {
                collectFramesReceived();
                break;
            }
            case FrameWritten:
            {
                emit Out(currentGlobalSignal);
                break;
            }
            default:
                break;
            }
        }
    }
}

void CanBusWorkerBasis::collectFramesReceived()
{
    if (currentDev->framesAvailable())
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
    }
}

void CanBusWorkerBasis::emitErrorGlobalSignal()
{
    anIf(CanBusWorkerBasisDbgEn,
         anError("Emit CanBusWorkerBasis::Error");
         anInfo("ErrorType: " + QString(CanBusWorkerBasis::ErrorMetaEnum.valueToKey(static_cast<int>(ErrorType))));
         anInfo("ErrorInfo: " + ErrorInfo);
    );
    GlobalSignal errorGlobalSignal;
    errorGlobalSignal.Type = QVariant::fromValue(ErrorType);
    errorGlobalSignal.Data = QVariant::fromValue(ErrorInfo);
    errorGlobalSignal.Priority = 200;
    errorGlobalSignal.SignalPriority = 200;
    errorGlobalSignal.DstStrs.append(SmallCoordinatorObjName);
    emit Out(errorGlobalSignal);
}

void CanBusWorkerBasis::In(const GlobalSignal &aGlobalSignal)
{
    addAGlobalSignal(aGlobalSignal);
}

const QMetaEnum CanBusWorkerBasis::DataMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Data>();
const QMetaEnum CanBusWorkerBasis::ErrorMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Error>();
const QMetaEnum CanBusWorkerBasis::WarningMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Warning>();
const QMetaEnum CanBusWorkerBasis::NotificationMetaEnum = QMetaEnum::fromType<CanBusWorkerBasis::Notification>();
const QMetaEnum CanBusWorkerBasis::QCanBusErrorMetaEnum = QMetaEnum::fromType<QCanBusDevice::CanBusError>();
const QMetaEnum CanBusWorkerBasis::QCanBusDeviceStateMetaEnum = QMetaEnum::fromType<QCanBusDevice::CanBusDeviceState>();
