/**
   @file hrmplugin.cpp
   @brief Plugin for heart rate monitor sensor

   <p>
   Copyright (C) 2009-2010 Nokia Corporation
   Copyright (C) 2019 Florent Revest

   @author Kimmo Lindholm <kimmo.lindholm@eke.fi>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>

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

#include "hrmplugin.h"
#include "hrmsensor.h"
#include "sensormanager.h"
#include "logging.h"

void HrmPlugin::Register(class Loader&)
{
    sensordLogD() << "registering hrmsensor";
    SensorManager& sm = SensorManager::instance();
    sm.registerSensor<HrmSensorChannel>("hrmsensor");
}

void HrmPlugin::Init(class Loader& l)
{
    Q_UNUSED(l);
    SensorManager::instance().requestSensor("hrmsensor");
}

QStringList HrmPlugin::Dependencies() {
    return QString("hrmadaptor").split(":", QString::SkipEmptyParts);
}
