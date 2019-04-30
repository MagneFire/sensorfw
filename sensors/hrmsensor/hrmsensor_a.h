/**
   @file hrmsensor_a.h
   @brief D-Bus adaptor for heart rate monitor sensor

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

#ifndef HRM_SENSOR_H
#define HRM_SENSOR_H

#include <QtDBus/QtDBus>
#include <QObject>

#include "datatypes/heartrate.h"
#include "abstractsensor_a.h"

class HrmSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(HrmSensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.HrmSensor")
    Q_PROPERTY(HeartRate heartRate READ heartRate)

public:
    HrmSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    HeartRate heartRate() const;

Q_SIGNALS:
    void HeartRateChanged(const HeartRate& value);
};

#endif
