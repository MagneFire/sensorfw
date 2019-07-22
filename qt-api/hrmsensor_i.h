/**
   @file hrmsensor_i.h
   @brief Interface for heart rate monitor sensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2019 Florent Revest

   @author Kimmo Lindholm <kimmo.lindholm@eke.fi>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>

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

#ifndef HRMSENSOR_I_H
#define HRMSENSOR_I_H

#include <QtDBus/QtDBus>

#include "datatypes/heartrate.h"
#include "abstractsensor_i.h"

/**
 * Client interface for accessing heart rate monitor sensor.
 * Provides signal on change of bpm of hrm status.
 * Previous measured heart rate can be queried any time. Provided
 * values are in \e heartRate.
 */
class HrmSensorChannelInterface : public AbstractSensorChannelInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(HrmSensorChannelInterface)
    Q_PROPERTY(HeartRate heartRate READ heartRate)

public:
    /**
     * Name of the D-Bus interface for this class.
     */
    static const char* staticInterfaceName;

    /**
     * Create new instance of the class.
     *
     * @param id Sensor ID.
     * @param sessionId Session ID.
     * @return Pointer to new instance of the class.
     */
    static AbstractSensorChannelInterface* factoryMethod(const QString& id, int sessionId);

    /**
     * Get latest heart rate measurements from sensor daemon.
     *
     * @return current bpm and hrm status.
     */
    HeartRate heartRate();

    /**
     * Constructor.
     *
     * @param path      path.
     * @param sessionId session ID.
     */
    HrmSensorChannelInterface(const QString& path, int sessionId);

    /**
     * Request a listening interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static const HrmSensorChannelInterface* listenInterface(const QString& id);

    /**
     * Request a control interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     * @deprecated use interface(const QString&) instead.
     */
    static HrmSensorChannelInterface* controlInterface(const QString& id);

    /**
     * Request an interface to the sensor.
     *
     * @param id sensor ID.
     * @return Pointer to interface, or NULL on failure.
     */
    static HrmSensorChannelInterface* interface(const QString& id);

protected:
    virtual bool dataReceivedImpl();

Q_SIGNALS:
    /**
     * Sent when measured bpm has changed.
     *
     * @param value bpm reading.
     */
    void HeartRateChanged(const HeartRate& value);
};

namespace local {
  typedef ::HrmSensorChannelInterface HrmSensor;
}

#endif
