/**
   @file wristgesturesensor.h
   @brief WristGestureSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2020 Darrel Griët

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Darrel Griët <idanlcontact@gmail.com>

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

#ifndef WRISTGESTURE_SENSOR_CHANNEL_H
#define WRISTGESTURE_SENSOR_CHANNEL_H

#include <QObject>

#include "deviceadaptor.h"
#include "abstractsensor.h"
#include "wristgesturesensor_a.h"
#include "dataemitter.h"
#include "datatypes/timedunsigned.h"
#include "datatypes/unsigned.h"

class Bin;
template <class TYPE> class BufferReader;
class FilterBase;

/**
 * @brief Sensor for accessing device wristgesture.
 *
 * Provides device wristgesture based on the direction of acceleration vector.
 * Threshold value (mG) is used to control the sensitivity of change from one
 * wristgesture into another. See #WristGestureInterpreter for details on threshold.
 */
class WristGestureSensorChannel :
        public AbstractSensorChannel,
        public DataEmitter<TimedUnsigned>
{
    Q_OBJECT;
    Q_PROPERTY(Unsigned wristgesture READ wristgesture);

public:

    /**
     * Factory method for WristGestureSensorChannel.
     * @return New WristGestureSensorChannel as AbstractSensorChannel*
     */
    static AbstractSensorChannel* factoryMethod(const QString& id)
    {
        WristGestureSensorChannel* sc = new WristGestureSensorChannel(id);
        new WristGestureSensorChannelAdaptor(sc);

        return sc;
    }

    /**
    * Property method returning current wristgesture.
    * @return Current wristgesture.
    */
    Unsigned wristgesture() const
    {
        return prevWristGesture;
    }

public Q_SLOTS:
    bool start();
    bool stop();

signals:
    /**
     * Sent whenever wristgesture interpretation has changed.
     * @param wristgesture New wristgesture.
     */
    void wristgestureChanged(const int& wristgesture);

protected:
    WristGestureSensorChannel(const QString& id);
    virtual ~WristGestureSensorChannel();

private:
    TimedUnsigned                         prevWristGesture;
    Bin*                             filterBin_;
    Bin*                             marshallingBin_;

    DeviceAdaptor*                   wristGestureAdaptor_;

    BufferReader<TimedUnsigned>*   wristgestureReader_;

    RingBuffer<TimedUnsigned>*            outputBuffer_;

    /**
     * Emits new device wristgesture through DBus.
     * @param value WristGesture value to emit.
     */
    void emitData(const TimedUnsigned& value);
};

#endif // WRISTGESTURE_SENSOR_CHANNEL_H
