/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
**
** $QT_BEGIN_LICENSE:LGPL$
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "hybrisadaptor.h"
#include "deviceadaptor.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTimer>

#include <hardware/hardware.h>
#include <hardware/sensors.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

#ifndef SENSOR_TYPE_ACCELEROMETER
#define SENSOR_TYPE_ACCELEROMETER (1)
#endif
#ifndef SENSOR_TYPE_MAGNETIC_FIELD
#define SENSOR_TYPE_MAGNETIC_FIELD (2)
#endif
#ifndef SENSOR_TYPE_ORIENTATION
#define SENSOR_TYPE_ORIENTATION (3)
#endif
#ifndef SENSOR_TYPE_GYROSCOPE
#define SENSOR_TYPE_GYROSCOPE (4)
#endif
#ifndef SENSOR_TYPE_LIGHT
#define SENSOR_TYPE_LIGHT (5)
#endif
#ifndef SENSOR_TYPE_PRESSURE
#define SENSOR_TYPE_PRESSURE (6)
#endif
#ifndef SENSOR_TYPE_TEMPERATURE
#define SENSOR_TYPE_TEMPERATURE (7)
#endif
#ifndef SENSOR_TYPE_PROXIMITY
#define SENSOR_TYPE_PROXIMITY (8)
#endif
#ifndef SENSOR_TYPE_GRAVITY
#define SENSOR_TYPE_GRAVITY (9)
#endif
#ifndef SENSOR_TYPE_LINEAR_ACCELERATION
#define SENSOR_TYPE_LINEAR_ACCELERATION (10)
#endif
#ifndef SENSOR_TYPE_ROTATION_VECTOR
#define SENSOR_TYPE_ROTATION_VECTOR (11)
#endif
#ifndef SENSOR_TYPE_RELATIVE_HUMIDITY
#define SENSOR_TYPE_RELATIVE_HUMIDITY (12)
#endif
#ifndef SENSOR_TYPE_AMBIENT_TEMPERATURE
#define SENSOR_TYPE_AMBIENT_TEMPERATURE (13)
#endif
//#define SENSOR_TYPE_MAGNETIC_FIELD_UNCALIBRATED (14)
//#define SENSOR_TYPE_GAME_ROTATION_VECTOR (15)
//#define SENSOR_TYPE_GYROSCOPE_UNCALIBRATED (16)
//#define SENSOR_TYPE_SIGNIFICANT_MOTION (17)
//#define SENSOR_TYPE_STEP_DETECTOR (18)
#ifndef SENSOR_TYPE_STEP_COUNTER
#define SENSOR_TYPE_STEP_COUNTER (19)
#endif
//#define SENSOR_TYPE_GEOMAGNETIC_ROTATION_VECTOR (20)

Q_GLOBAL_STATIC(HybrisManager, hybrisManager)

HybrisManager::HybrisManager(QObject *parent)
    : QObject(parent)
    , device(NULL)
    , sensorList(NULL)
    , module(NULL)
    , sensorsCount(0)
    , sensorMap()
    , registeredAdaptors()
    , adaptorReaderTid(0)
{
    init();
}

HybrisManager::~HybrisManager()
{
    closeAllSensors();
    if (adaptorReaderTid) {
        sensordLogD() << "Canceling hal reader thread";
        int err = pthread_cancel(adaptorReaderTid);
        if( err ) {
            sensordLogC() << "Failed to cancel hal reader thread";
        }
        else {
            sensordLogD() << "Waiting for hal reader thread to exit";
            void *ret = 0;
            struct timespec tmo = { 0, 0};
            clock_gettime(CLOCK_REALTIME, &tmo);
            tmo.tv_sec += 3;
            err = pthread_timedjoin_np(adaptorReaderTid, &ret, &tmo);
            if( err ) {
                sensordLogC() << "Hal reader thread did not exit";
            } else {
                sensordLogD() << "Hal reader thread terminated";
                adaptorReaderTid = 0;
            }
        }
        if (adaptorReaderTid) {
            /* The reader thread is stuck at android hal blob.
             * Continuing would be likely to release resourse
             * still in active use and lead to segfaulting.
             * Resort to doing a quick and dirty exit. */
            _exit(EXIT_FAILURE);
        }
    }
}

HybrisManager *HybrisManager::instance()
{
    HybrisManager *priv = hybrisManager();
    return priv;
}

void HybrisManager::init()
{
    int errorCode = hw_get_module(SENSORS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (errorCode != 0) {
        qDebug() << "hw_get_module() failed" <<  strerror(-errorCode);
        return ;
    }

    if (!openSensors()) {
        sensordLogW() << "Cannot open sensors";
        return;
    }

    sensorsCount = module->get_sensors_list(module, &sensorList);

    if (device) {
        sensordLogW() << Q_FUNC_INFO
            << "Deactivating all sensors";
        for (int i=0 ; i < sensorsCount ; i++) {
            device->activate(
                    reinterpret_cast<struct sensors_poll_device_t *>(device),
                    sensorList[i].handle, 0);
        }
    }

    for (int i = 0 ; i < sensorsCount ; i++) {
        bool use = true;
        // Assumption: The primary sensor variants that we want to
        // use are listed before the secondary ones that we want
        // to ignore -> Use the 1st entry found for each sensor type.
        if( sensorMap.contains(sensorList[i].type) ) {
            use = false;
        }

        // some devices have compass and compass raw,
        // ignore compass raw. compass has range 360
        if (sensorList[i].type == SENSOR_TYPE_ORIENTATION &&
            sensorList[i].maxRange != 360) {
            use = false;
        }

        sensordLogW() << Q_FUNC_INFO
            << (use ? "SELECT" : "IGNORE")
            << "type:" << sensorList[i].type
            << "name:" << (sensorList[i].name ?: "n/a");

        if (use) {
            sensorMap.insert(sensorList[i].type, i);
        }
    }

    int err = pthread_create(&adaptorReaderTid, 0, adaptorReaderThread, this);
    if (err) {
        adaptorReaderTid = 0;
        sensordLogC() << "Failed to start hal reader thread";
    } else {
        sensordLogD() << "Hal reader thread started";
    }
}

int HybrisManager::handleForType(int sensorType)
{
    if (sensorMap.contains(sensorType))
        return sensorList[sensorMap[sensorType]].handle;

    sensordLogW() << Q_FUNC_INFO << "No sensor of type:" << sensorType;
    return -1;
}

int HybrisManager::maxRange(int sensorType)
{
    if (sensorMap.contains(sensorType))
        return sensorList[sensorMap[sensorType]].maxRange;
    return 0;
}

int HybrisManager::minDelay(int sensorType)
{
    int res = 0;
    if (sensorMap.contains(sensorType)) {
        res = sensorList[sensorMap[sensorType]].minDelay;
        // us -> ms, round up
        res = (res + 999) / 1000;
    }
    return res;
}

int HybrisManager::resolution(int sensorType)
{
    if (sensorMap.contains(sensorType))
        return sensorList[sensorMap[sensorType]].resolution;
    return 0;
}

bool HybrisManager::setDelay(int sensorHandle, int interval)
{
    bool ok = true;
    if (interval > 0) {
        int result = device->setDelay(reinterpret_cast<struct sensors_poll_device_t *>(device), sensorHandle, interval);
        if (result < 0) {
            sensordLogW() << "setDelay() failed" << strerror(-result);
            ok = false;
        }
    }
    QList <HybrisAdaptor *> list;
    list = registeredAdaptors.values();
    for (int i = 0; i < list.count(); i++) {
        if (list.at(i)->sensorHandle == sensorHandle) {
            list.at(i)->sendInitialData();
        }
    }

    return ok;
}

void HybrisManager::startReader(HybrisAdaptor *adaptor)
{
    if (registeredAdaptors.values().contains(adaptor)) {
        sensordLogD() << "activating " << adaptor->name() << adaptor->sensorHandle;
        int error = device->activate(reinterpret_cast<struct sensors_poll_device_t *>(device), adaptor->sensorHandle, 1);
        if (error != 0) {
            sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
            adaptor->setValid(false);
        }
    }
}

void HybrisManager::stopReader(HybrisAdaptor *adaptor)
{
    QList <HybrisAdaptor *> list;
    list = registeredAdaptors.values();
    bool okToStop = true;

    for (int i = 0; i < list.count(); i++) {
        if (list.at(i) == adaptor && !list.at(i)->isRunning()) {
            sensordLogD() << "deactivating " << adaptor->name();
            int error = device->activate(reinterpret_cast<struct sensors_poll_device_t *>(device), adaptor->sensorHandle, 0);
            if (error != 0) {
                sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
            }
        }
        if (list.at(i) != adaptor && list.at(i)->shouldBeRunning_) {
            okToStop = false;
        }
    }
    qDebug() << "okToStop" << okToStop;
}

bool HybrisManager::resumeReader(HybrisAdaptor *adaptor)
{
    sensordLogD() << Q_FUNC_INFO << adaptor->id()
                  << adaptor->deviceStandbyOverride()
                  << adaptor->isRunning(); //alwaysOn

    if (!adaptor->isRunning()) {
        sensordLogD() << "activating for resume" << adaptor->name();
        int error = device->activate(reinterpret_cast<struct sensors_poll_device_t *>(device), adaptor->sensorHandle, 1);
        if (error != 0) {
            sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
        }
    }
    return true;
}

void HybrisManager::standbyReader(HybrisAdaptor *adaptor)
{
    sensordLogD() << Q_FUNC_INFO  << adaptor->id()
                  << adaptor->deviceStandbyOverride()
                  << adaptor->isRunning(); //alwaysOn

    if (adaptor->isRunning() && !adaptor->deviceStandbyOverride()) {
        sensordLogD() << "deactivating for standby" << adaptor->name();
        int error = device->activate(reinterpret_cast<struct sensors_poll_device_t *>(device), adaptor->sensorHandle, 0);
        if (error != 0) {
            sensordLogW() <<Q_FUNC_INFO<< "failed for"<< strerror(-error);
        }
    }
}

bool HybrisManager::openSensors()
{
    if (!device) {
        sensordLogD() << "Calling sensors_open";
        int errorCode = sensors_open_1(&module->common, &device);
        if (errorCode != 0) {
            sensordLogW() << "sensors_open() failed:" << strerror(-errorCode);
            device = NULL;
        }
    }

    return (device != NULL);
}

bool HybrisManager::closeSensors()
{
    if (device) {
        foreach (HybrisAdaptor *adaptor, registeredAdaptors.values()) {
            if (adaptor->isRunning()) {
                sensordLogW() << Q_FUNC_INFO << "still running:" << adaptor;
                return false;
            }
        }

        sensordLogD() << "Calling sensors_close";
        int errorCode = sensors_close_1(device);
        if (errorCode != 0) {
            sensordLogW() << "sensors_close() failed:" << strerror(-errorCode);
        }
    }

    device = NULL;
    return true;
}

void HybrisManager::closeAllSensors()
{
    QList <HybrisAdaptor *> list;
    list = registeredAdaptors.values();

    for (int i = 0; i < list.count(); i++) {
        if (list.at(i)->isRunning())
            list.at(i)->stopSensor();
    }

    if (!closeSensors()) {
        sensordLogW() << "Cannot close sensors";
    }
}

void HybrisManager::processSample(const sensors_event_t& data)
{
    foreach (HybrisAdaptor *adaptor, registeredAdaptors.values(data.type)) {
        if (adaptor->isRunning()) {
            adaptor->processSample(data);
        }
    }
}

void HybrisManager::registerAdaptor(HybrisAdaptor *adaptor)
{
    if (!registeredAdaptors.values().contains(adaptor) && adaptor->isValid()) {
        registeredAdaptors.insertMulti(adaptor->sensorType, adaptor);
    }
}

//////////////////////////////////
HybrisAdaptor::HybrisAdaptor(const QString& id, int type)
    : DeviceAdaptor(id),
      sensorType(type),
      cachedInterval(50),
      inStandbyMode_(false),
      running_(false)
{
    sensorHandle = hybrisManager()->handleForType(sensorType);
    if (sensorHandle == -1) {
        qDebug() << Q_FUNC_INFO <<"no such sensor" << id;
        setValid(false);
        return;
    }

    hybrisManager()->registerAdaptor(this);
    init();
    introduceAvailableInterval(DataRange(minDelay, 1000, 0));
    introduceAvailableDataRange(DataRange(-(maxRange*.5), (maxRange*.5), 1));
}

HybrisAdaptor::~HybrisAdaptor()
{
}

void HybrisAdaptor::init()
{
    maxRange = hybrisManager()->maxRange(sensorType);
    minDelay = hybrisManager()->minDelay(sensorType);
    if (minDelay > 1000)
        minDelay = 0;
    resolution = hybrisManager()->resolution(sensorType);
}

bool HybrisAdaptor::addSensorType(int type)
{
    sensorIds.append(type);
    return true;
}

bool HybrisAdaptor::startAdaptor()
{
    if (!isValid())
        return false;
    return hybrisManager()->openSensors();
}

bool HybrisAdaptor::isRunning() const
{
    return running_;
}

void HybrisAdaptor::stopAdaptor()
{
    if (getAdaptedSensor()->isRunning())
        stopSensor();
    hybrisManager()->closeSensors();
}

bool HybrisAdaptor::startSensor()
{
    AdaptedSensorEntry *entry = getAdaptedSensor();
    if (entry == NULL) {
        qDebug() << "Sensor not found: " << name();
        return false;
    }

    // Increase listener count
    entry->addReference();

    /// Check from entry
    if (isRunning()) {
        qDebug()  << Q_FUNC_INFO << "already running";
        shouldBeRunning_ = true;
        return false;
    }

    // Do not open if in standby mode.
    if (inStandbyMode_ && !deviceStandbyOverride()) {
        qDebug()  << Q_FUNC_INFO << "inStandbyMode_ true";
        return false;
    }

    shouldBeRunning_ = true;
    /// We are waking up from standby or starting fresh, no matter
    inStandbyMode_ = false;

    if (!startReaderThread()) {
        entry->removeReference();
        entry->setIsRunning(false);
        running_ = false;
        shouldBeRunning_ = false;
        return false;
    }

    entry->setIsRunning(true);
    running_ = true;
    shouldBeRunning_ = true;

    return true;
}

void HybrisAdaptor::stopSensor()
{
    AdaptedSensorEntry *entry = getAdaptedSensor();

    if (entry == NULL) {
        sensordLogW() << "Sensor not found " << name();
        return;
    }
    qDebug() << "shouldBeRunning_" << shouldBeRunning_
             << "inStandbyMode_" << inStandbyMode_
             << "reference count" << entry->referenceCount();

    if (!shouldBeRunning_) {
        return;
    }

    entry->removeReference();
    if (entry->referenceCount() <= 0) {
        entry->setIsRunning(false);
        running_ = false;
        shouldBeRunning_ = false;
        inStandbyMode_ = false;
        if (!inStandbyMode_) {
            stopReaderThread();
        }
    }
}

bool HybrisAdaptor::standby()
{
    sensordLogD() << "Adaptor '" << id() << "' requested to go to standby"  << "deviceStandbyOverride" << deviceStandbyOverride();
    if (inStandbyMode_ && deviceStandbyOverride()) {
        sensordLogD() << "Adaptor '" << id() << "' not going to standby: already in standby";
        return false;
    }

    if (!isRunning()) {
        sensordLogD() << "Adaptor '" << id() << "' not going to standby: not running";
        return false;
    }

    inStandbyMode_ = true;
    hybrisManager()->standbyReader(this);
    running_ = deviceStandbyOverride();

    return true;
}

bool HybrisAdaptor::resume()
{
    sensordLogD() << "Adaptor '" << id() << "' requested to resume from standby";
    sensordLogD() << "deviceStandbyOverride" << deviceStandbyOverride();

    // Don't resume if not in standby
    if (!inStandbyMode_ && !deviceStandbyOverride()) {
        sensordLogD() << "Adaptor '" << id() << "' not resuming: not in standby";
        return false;
    }

    if (!shouldBeRunning_) {
        sensordLogD() << "Adaptor '" << id() << "' not resuming from standby: not running";
        return false;
    }

    sensordLogD() << "Adaptor '" << id() << "' resuming from standby";
    inStandbyMode_ = false;

    if (!hybrisManager()->resumeReader(this)) {
        sensordLogW() << "Adaptor '" << id() << "' failed to resume from standby!";
        return false;
    }
    running_ = true;

    return true;
}

unsigned int HybrisAdaptor::interval() const
{
    return cachedInterval;
}

bool HybrisAdaptor::setInterval(const unsigned int value, const int /*sessionId*/)
{                     // 1000000
    cachedInterval = value;
    bool ok;
    qreal ns = value * 1000000; // ms to ns
    ok = hybrisManager()->setDelay(sensorHandle, ns);
    if (!ok) {
        qDebug() << Q_FUNC_INFO << "setInterval not ok";
    }
    return ok;
}

void HybrisAdaptor::stopReaderThread()
{
    hybrisManager()->stopReader(this);
    running_ = false;
}

bool HybrisAdaptor::startReaderThread()
{
    running_ = true;
    hybrisManager()->startReader(this);
    return true;
}

unsigned int HybrisAdaptor::evaluateIntervalRequests(int& sessionId) const
{
    if (m_intervalMap.size() == 0)
    {
        sessionId = -1;
        return defaultInterval();
    }

    // Get the smallest positive request, 0 is reserved for HW wakeup
    QMap<int, unsigned int>::const_iterator it = m_intervalMap.constBegin();
    unsigned int highestValue = it.value();
    int winningSessionId = it.key();

    for (++it; it != m_intervalMap.constEnd(); ++it)
    {
        if (((it.value() < highestValue) && (it.value() > 0)) || highestValue == 0) {
            highestValue = it.value();
            winningSessionId = it.key();
        }
    }

    sessionId = winningSessionId;
    return highestValue > 0 ? highestValue : defaultInterval();
}

bool HybrisAdaptor::writeToFile(const QByteArray& path, const QByteArray& content)
{
    sensordLogT() << "Writing to '" << path << ": " << content;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        sensordLogW() << "Failed to open '" << path << "': " << file.errorString();
        return false;
    }
    if (file.write(content.constData(), content.size()) == -1)
    {
        sensordLogW() << "Failed to write to '" << path << "': " << file.errorString();
        file.close();
        return false;
    }

    file.close();
    return true;
}

static void ObtainTemporaryWakeLock()
{
    static bool triedToOpen = false;
    static int wakeLockFd = -1;

    if (!triedToOpen) {
        triedToOpen = true;
        wakeLockFd = ::open("/sys/power/wake_lock", O_RDWR);
        if (wakeLockFd == -1) {
            sensordLogW() << "wake locks not available:" << ::strerror(errno);
        }
    }

    if (wakeLockFd != -1) {
        sensordLogD() << "wake lock to guard sensor data io";
        static const char m[] = "sensorfwd_pass_data 1000000000\n";
        if( ::write(wakeLockFd, m, sizeof m - 1) == -1 ) {
            sensordLogW() << "wake locking failed:" << ::strerror(errno);
            ::close(wakeLockFd), wakeLockFd = -1;
        }
    }
}

void *HybrisManager::adaptorReaderThread(void *aptr)
{
    HybrisManager *manager = static_cast<HybrisManager *>(aptr);
    static const size_t numEvents = 16;
    sensors_event_t buffer[numEvents];
    /* Async cancellation, but disabled */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
    /* Leave INT/TERM signal processing up to the main thread */
    sigset_t ss;
    sigemptyset(&ss);
    sigaddset(&ss, SIGINT);
    sigaddset(&ss, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &ss, 0);
    /* Loop until explicitly canceled */
    for( ;; ) {
        /* Async cancellation point at android hal poll() */
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
        int numberOfEvents = manager->device->poll(reinterpret_cast<struct sensors_poll_device_t *>(manager->device), buffer, numEvents);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
        /* Rate limit in poll() error situations */
        if (numberOfEvents < 0) {
            sensordLogW() << "android device->poll() failed" << strerror(-numberOfEvents);
            struct timespec ts = { 1, 0 }; // 1000 ms
            do { } while( nanosleep(&ts, &ts) == -1 && errno == EINTR );
            continue;
        }
        /* Process received events */
        bool blockSuspend = false;
        bool errorInInput = false;
        for (int i = 0; i < numberOfEvents; i++) {
            const sensors_event_t& data = buffer[i];
            if (data.version != sizeof(sensors_event_t)) {
                sensordLogW()<< QString("incorrect event version (version=%1, expected=%2").arg(data.version).arg(sizeof(sensors_event_t));
                errorInInput = true;
            }
            if (data.type == SENSOR_TYPE_PROXIMITY) {
                blockSuspend = true;
            }
            // FIXME: is this thread safe?
            manager->processSample(data);
        }
        /* Suspend proof sensor reporting that could occur in display off */
        if (blockSuspend) {
            ObtainTemporaryWakeLock();
        }
        /* Rate limit after receiving erraneous events */
        if (errorInInput) {
            struct timespec ts = { 0, 50 * 1000 * 1000 }; // 50 ms
            do { } while( nanosleep(&ts, &ts) == -1 && errno == EINTR );
        }
    }
    return 0;
}
