/**
   @file hrmsensor.h
   @brief heart rate monitor sensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2019 Florent Revest

   @author Kimmo Lindholm <kimmo.lindholm@eke.fi>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>

   This file is part of Sensord.

   Sensord is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License
   version 2.1 as published by the Free Software Foundation.

   Sensord is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with Sensord.  If not, see <http://www.gnu.org/licenses/>.
   </p>
 */

#ifndef HRM_SENSOR_CHANNEL_H
#define HRM_SENSOR_CHANNEL_H

#include <QObject>

#include "deviceadaptor.h"
#include "abstractsensor.h"
#include "hrmsensor_a.h"
#include "dataemitter.h"
#include "datatypes/heartratedata.h"
#include "datatypes/heartrate.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor for accessing the internal heart rate monitor sensor measurements.
 *
 * Signals listeners whenever observed bpm changed.
 */
class HrmSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<HeartRateData>
{
    Q_OBJECT;
    Q_PROPERTY(HeartRate heartRate READ heartRate);

public:
    /**
     * Factory method for HrmSensorChannel.
     * @return New HrmSensorChannel as AbstractSensorChannel*
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        HrmSensorChannel* sc = new HrmSensorChannel(id);
        new HrmSensorChannelAdaptor(sc);

        return sc;
    }

    /**
     * Property for accessing the measured value.
     * @return Last measured value.
     */
    HeartRate heartRate() const { return previousValue_; }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent when a change in measured data is observed.
     * @param value Measured value.
     */
    void HeartRateChanged(const HeartRate& value);

protected:
    HrmSensorChannel(const QString& id);
    virtual ~HrmSensorChannel();

private:
    HeartRateData                 previousValue_;
    Bin*                          filterBin_;
    Bin*                          marshallingBin_;
    DeviceAdaptor*                hrmAdaptor_;
    BufferReader<HeartRateData>*  hrmReader_;
    RingBuffer<HeartRateData>*    outputBuffer_;

    void emitData(const HeartRateData& value);
};

#endif // HRM_SENSOR_CHANNEL_H
