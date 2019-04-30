/**
   @file heartrate.cpp
   @brief QObject based datatype for HeartRateData

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
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

#include "heartrate.h"

HeartRate::HeartRate(const HeartRateData& heartRateData)
    : QObject(), data_(heartRateData.timestamp_, heartRateData.status_, heartRateData.bpm_)
{
}

HeartRate::HeartRate(const HeartRate& heartRate)
    : QObject(), data_(heartRate.heartRateData().timestamp_, heartRate.heartRateData().status_, heartRate.heartRateData().bpm_)
{
}
