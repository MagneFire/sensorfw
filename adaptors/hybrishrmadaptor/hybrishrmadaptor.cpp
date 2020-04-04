/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
** Copyright (C) 2019 Florent Revest
** Contact: revestflo@gmail.com
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

#include "hybrishrmadaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <hardware/sensors.h>
#include "config.h"

#ifndef SENSOR_TYPE_HEART_RATE
#define SENSOR_TYPE_HEART_RATE (21)
#endif

HybrisHrmAdaptor::HybrisHrmAdaptor(const QString& id) :
    HybrisAdaptor(id, SENSOR_TYPE_HEART_RATE)
{
    buffer = new DeviceAdaptorRingBuffer<HeartRateData>(1);
    setAdaptedSensor("hrm", "Heart rate monitor", buffer);
    setDescription("Hybris heart rate monitor");
    powerStatePath = SensorFrameworkConfig::configuration()->value("heartrate/powerstate_path").toByteArray();
    if (!powerStatePath.isEmpty() && !QFile::exists(powerStatePath))
    {
    	sensordLogW() << "Path does not exists: " << powerStatePath;
    	powerStatePath.clear();
    }
}

HybrisHrmAdaptor::~HybrisHrmAdaptor()
{
    delete buffer;
}

bool HybrisHrmAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;
    if (isRunning() && !powerStatePath.isEmpty())
        writeToFile(powerStatePath, "1");
    sensordLogD() << "Hybris HybrisHrmAdaptor start\n";
    return true;
}

void HybrisHrmAdaptor::sendInitialData()
{
    sensordLogW() << "No initial data for heart rate monitor";
}

void HybrisHrmAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
    if (!isRunning() && !powerStatePath.isEmpty())
        writeToFile(powerStatePath, "0");
    sensordLogD() << "Hybris HybrisHrmAdaptor stop\n";
}

void HybrisHrmAdaptor::processSample(const sensors_event_t& data)
{
    HeartRateData *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
    d->bpm_ = data.heart_rate.bpm;
    switch(data.heart_rate.status) {
    case SENSOR_STATUS_UNRELIABLE:
        d->status_ = HrmUnreliable;
        break;
    case SENSOR_STATUS_ACCURACY_LOW:
        d->status_ = HrmAccuracyLow;
        break;
    case SENSOR_STATUS_ACCURACY_MEDIUM:
        d->status_ = HrmAccuracyMedium;
        break;
    case SENSOR_STATUS_ACCURACY_HIGH:
        d->status_ = HrmAccuracyHigh;
        break;
    default:
        d->status_ = HrmNoContact;
    }
    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisHrmAdaptor::init()
{
}
