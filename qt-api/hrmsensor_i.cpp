/**
   @file hrmsensor_i.cpp
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

#include "sensormanagerinterface.h"
#include "hrmsensor_i.h"
#include "socketreader.h"

const char* HrmSensorChannelInterface::staticInterfaceName = "local.HrmSensor";

AbstractSensorChannelInterface* HrmSensorChannelInterface::factoryMethod(const QString& id, int sessionId)
{
    return new HrmSensorChannelInterface(OBJECT_PATH + "/" + id, sessionId);
}

HrmSensorChannelInterface::HrmSensorChannelInterface(const QString& path, int sessionId)
    : AbstractSensorChannelInterface(path, HrmSensorChannelInterface::staticInterfaceName, sessionId)
{
}

const HrmSensorChannelInterface* HrmSensorChannelInterface::listenInterface(const QString& id)
{
    return dynamic_cast<const HrmSensorChannelInterface*> (interface(id));
}

HrmSensorChannelInterface* HrmSensorChannelInterface::controlInterface(const QString& id)
{
    return interface(id);
}


HrmSensorChannelInterface* HrmSensorChannelInterface::interface(const QString& id)
{
    SensorManagerInterface& sm = SensorManagerInterface::instance();
    if ( !sm.registeredAndCorrectClassName( id, HrmSensorChannelInterface::staticMetaObject.className() ) )
    {
        return 0;
    }

    return dynamic_cast<HrmSensorChannelInterface*>(sm.interface(id));
}

bool HrmSensorChannelInterface::dataReceivedImpl()
{
    QVector<HeartRate> values;
    if(!read<HeartRate>(values))
        return false;
    foreach(const HeartRate& data, values)
        emit HrmChanged(data);
    return true;
}

HeartRate HrmSensorChannelInterface::heartRate()
{
    return getAccessor<HeartRate>("heartRate");
}
