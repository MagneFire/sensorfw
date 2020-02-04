/**
   @file wristgesturesensor_a.cpp
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

#include "wristgesturesensor_a.h"

WristGestureSensorChannelAdaptor::WristGestureSensorChannelAdaptor(QObject* parent) :
    AbstractSensorChannelAdaptor(parent)
{
}

Unsigned WristGestureSensorChannelAdaptor::wristgesture() const
{
    return qvariant_cast<Unsigned>(parent()->property("wristgesture"));
}

int WristGestureSensorChannelAdaptor::threshold() const
{
    return qvariant_cast<int>(parent()->property("threshold"));
}

void WristGestureSensorChannelAdaptor::setThreshold(int value)
{
    parent()->setProperty("threshold", value);
}
