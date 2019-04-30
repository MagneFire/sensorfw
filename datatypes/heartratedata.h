/**
   @file heartratedata.h
   @brief Datatypes for different filters

   <p>
   Copyright (C) 2009-2010 Nokia Corporation

   @author Joep van Gassel <joep.van.gassel@nokia.com>
   @author Timo Rongas <ext-timo.2.rongas@nokia.com>
   @author Ustun Ergenoglu <ext-ustun.ergenoglu@nokia.com>
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

#ifndef HEARTRATEDATA_H
#define HEARTRATEDATA_H

#include <datatypes/genericdata.h>

/**
 * Status
 */
enum HrmStatus
{
    HrmNoContact,
    HrmUnreliable,
    HrmAccuracyLow,
    HrmAccuracyMedium,
    HrmAccuracyHigh
};
Q_DECLARE_METATYPE ( HrmStatus )

inline QDBusArgument &operator<<(QDBusArgument &argument, HrmStatus value)
{
    argument.beginStructure();
    qlonglong newVal = (qlonglong)value;
    argument << newVal;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, HrmStatus &val)
{
    argument.beginStructure();
    qlonglong result = 0;
    argument >> result;
    val = (HrmStatus)result;
    argument.endStructure();
    return argument;
}

/**
 * Class for vector type measurement data (timestamp, x, y, z).
 */
class HeartRateData : public TimedData
{
public:
    /**
     * Constructor.
     */
    HeartRateData() : TimedData(0), status_(HrmNoContact), bpm_(0) {}

    /**
     * Constructor.
     *
     * @param timestamp monotonic time (microsec)
     * @param bpm    Beats per minute.
     * @param status Heart Rate Monitor status.
     */
    HeartRateData(const quint64& timestamp, HrmStatus status, int bpm) : TimedData(timestamp), status_(status), bpm_(bpm) {}

    HrmStatus status_; /**< hrm status */
    int bpm_; /**< beat per minute */
};
Q_DECLARE_METATYPE ( HeartRateData )

#endif // HEARTRATEDATA_H
