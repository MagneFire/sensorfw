/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd
** Contact: lorn.potter@jollamobile.com
**
** Copyright (C) 2020 Darrel Griët
** Contact: idanlcontact@gmail.com
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

#include "hybriswristgestureadaptor.h"
#include "logging.h"
#include "datatypes/utils.h"
#include <hardware/sensors.h>
#include "config.h"


#ifndef SENSOR_TYPE_WRIST_TILT_GESTURE
#define SENSOR_TYPE_WRIST_TILT_GESTURE (26)
#endif

HybrisWristGestureAdaptor::HybrisWristGestureAdaptor(const QString& id) :
    HybrisAdaptor(id,SENSOR_TYPE_WRIST_TILT_GESTURE)
{
    buffer = new DeviceAdaptorRingBuffer<TimedUnsigned>(1);
    setAdaptedSensor("hybriswristgesture", "Internal wristgesture coordinates", buffer);

    setDescription("Hybris wristgesture");
    powerStatePath = SensorFrameworkConfig::configuration()->value("wristgesture/powerstate_path").toByteArray();
    if (!powerStatePath.isEmpty() && !QFile::exists(powerStatePath))
    {
    	sensordLogW() << "Path does not exists: " << powerStatePath;
    	powerStatePath.clear();
    }

    // Set default delay.
    setInterval(200, 0);
}

HybrisWristGestureAdaptor::~HybrisWristGestureAdaptor()
{
    delete buffer;
}

bool HybrisWristGestureAdaptor::startSensor()
{
    if (!(HybrisAdaptor::startSensor()))
        return false;
    if (isRunning() && !powerStatePath.isEmpty())
        writeToFile(powerStatePath, "1");
    sensordLogD() << "Hybris WristGestureAdaptor start\n";
    return true;
}

void HybrisWristGestureAdaptor::stopSensor()
{
    HybrisAdaptor::stopSensor();
     if (!isRunning() && !powerStatePath.isEmpty())
        writeToFile(powerStatePath, "0");
    sensordLogD() << "Hybris WristGestureAdaptor stop\n";
}

void HybrisWristGestureAdaptor::processSample(const sensors_event_t& data)
{
    TimedUnsigned *d = buffer->nextSlot();
    d->timestamp_ = quint64(data.timestamp * .001);
#ifdef NO_SENSORS_EVENT_U64
    uint64_t value = 0;
    memcpy(&value, data.data, sizeof value);
    d->value_ = value;
#else
    d->value_ = data.u64.step_counter;
#endif
    sensordLogD() << "HybrisWristGestureAdaptor: processSample(): " << data.data[0];

    buffer->commit();
    buffer->wakeUpReaders();
}

void HybrisWristGestureAdaptor::init()
{
}
