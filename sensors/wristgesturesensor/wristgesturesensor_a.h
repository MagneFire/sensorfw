/**
   @file wristgesturesensor_a.h
   @brief D-Bus Adaptor for WristGestureSensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2020 Darrel Griët

   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Antti Virtanen <antti.i.virtanen@nokia.com>
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

#ifndef WRISTGESTURE_SENSOR_H
#define WRISTGESTURE_SENSOR_H

#include <QtDBus/QtDBus>

#include "datatypes/unsigned.h"
#include "abstractsensor_a.h"

class WristGestureSensorChannelAdaptor : public AbstractSensorChannelAdaptor
{
    Q_OBJECT
    Q_DISABLE_COPY(WristGestureSensorChannelAdaptor)
    Q_CLASSINFO("D-Bus Interface", "local.WristGestureSensor")
    Q_PROPERTY(Unsigned wristgesture READ wristgesture)
    Q_PROPERTY(int threshold READ threshold WRITE setThreshold)

public:
    WristGestureSensorChannelAdaptor(QObject* parent);

public Q_SLOTS:
    Unsigned wristgesture() const;
    int threshold() const;
    void setThreshold(int value);

Q_SIGNALS:
    void wristgestureChanged(const Unsigned& wristgesture);
};

#endif
